/*********************************************************************
*  Author  : Himangshu Saikia
*  Init    : Tuesday, September 19, 2017 - 15:08:33
*
*  Project : KTH Inviwo Modules
*
*  License : Follows the Inviwo BSD license model
*********************************************************************
*/

#include <labstreamlines/streamlineintegrator.h>
#include <labstreamlines/integrator.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/interaction/events/mouseevent.h>

namespace inviwo {

	// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
	const ProcessorInfo StreamlineIntegrator::processorInfo_{
		"org.inviwo.StreamlineIntegrator",  // Class identifier
		"Streamline Integrator",            // Display name
		"KTH Lab",                          // Category
		CodeState::Experimental,            // Code state
		Tags::None,                         // Tags
	};

	const ProcessorInfo StreamlineIntegrator::getProcessorInfo() const { return processorInfo_; }

	StreamlineIntegrator::StreamlineIntegrator()
		: Processor()
		, inData("volIn")
		, outMesh("meshOut")
		, propStartPoint("startPoint", "Start Point", vec2(0.5, 0.5), vec2(0), vec2(1024), vec2(0.5))
		, propSeedMode("seedMode", "Seeds")
		// TODO: Initialize additional properties
		// propertyName("propertyIdentifier", "Display Name of the Propery",
		// default value (optional), minimum value (optional), maximum value (optional), increment
		// (optional)); propertyIdentifier cannot have spaces

		//Jane: add new properties
		, propDirection("direction", "Direction of Integration")
		, propSeeding("seeding", "Seeding Strategy")
		, nseeds("howmanyseeds", "How Many Seeds", 200, 1, 500)
		, gridx("sizeofgridonx", "Size of Grid on X-axis", 25, 1, 100)
		, gridy("sizeofgridony", "Size of Grid on Y-axis", 25, 1, 100)
		, propfield("field", "Field")
		, propStepSize("stepSize", "Step Size of Isolines", 0.10, 0.00, 0.50)
		, numberOfSteps("stepCount", "Number of Steps in One Direction", 200, 1, 1000)
		, propmax("maximumLength", "Maximum Length of Streamline", 1000, 1, 1000)
		, propmin("earlyTermination", "Terminate Propogation When Segment is Less Than...", 0.08, 0.00, 0.20)
		, mouseMoveStart("mouseMoveStart", "Move Start", [this](Event* e) { eventMoveStart(e); },
			MouseButton::Left, MouseState::Press | MouseState::Move) {
		// Register Ports
		addPort(inData);
		addPort(outMesh);

		// Register Properties
		propSeedMode.addOption("one", "Single Start Point", 0);
		propSeedMode.addOption("multiple", "Multiple Seeds", 1);
		addProperty(propSeedMode);
		addProperty(propStartPoint);
		addProperty(mouseMoveStart);

		//Jane: add new properties
		propDirection.addOption("forward", "Forward", 0);
		propDirection.addOption("backward", "Backward", 1);
		addProperty(propDirection);
		propfield.addOption("directionfield", "Direction Field", 1);
		propfield.addOption("vectorfield", "Vector Field", 0);
		addProperty(propfield);
		addProperty(propStepSize);
		addProperty(numberOfSteps);
		addProperty(propmin);
		addProperty(propmax);

		//Hara: multiseed properties
		propSeeding.addOption("grid", "On Uniform Grid", 1);
		propSeeding.addOption("random", "Random", 0);
		//addProperty(propSeeding);
		addProperty(nseeds);
		addProperty(gridx);
		addProperty(gridy);


		// TODO: Register additional properties
		// addProperty(propertyName);

		// You can hide and show properties for a single seed and hide properties for multiple seeds (TODO)
		propSeedMode.onChange([this]() {
			if (propSeedMode.get() == 0) {
				util::show(propStartPoint, mouseMoveStart, propDirection);
				util::hide(gridx, gridy, nseeds);
			}
			else {
				util::hide(propStartPoint, mouseMoveStart, propDirection);
				if (propSeeding.get() == 1) {
					util::show(gridx, gridy);
					util::hide(nseeds);
				}
				else {
					util::hide(gridx, gridy);
					util::show(nseeds);
				}
			}
		});

		propSeeding.onChange([this]() {
			if (propSeeding.get() == 1) {
				util::show(gridx, gridy);
				util::hide(nseeds);
			}
			else {
				util::hide(gridx, gridy);
				util::show(nseeds);
			}
		});

	}

	void StreamlineIntegrator::eventMoveStart(Event* event) {
		// Handle mouse interaction only if we
		// are in the mode with a single point
		if (propSeedMode.get() == 1) return;
		auto mouseEvent = static_cast<MouseEvent*>(event);
		vec2 mousePos = mouseEvent->posNormalized();
		// Denormalize to volume dimensions
		mousePos.x *= dims.x - 1;
		mousePos.y *= dims.y - 1;
		// Update starting point
		propStartPoint.set(mousePos);
		event->markAsUsed();
	}

	void StreamlineIntegrator::process() {
		// Get input
		if (!inData.hasData()) {
			return;
		}
		auto vol = inData.getData();

		// Retreive data in a form that we can access it
		auto vr = vol->getRepresentation<VolumeRAM>();
		dims = vol->getDimensions();
		// The start point should be inside the volume (set maximum to the upper right corner)
		propStartPoint.setMaxValue(vec2(dims.x - 1, dims.y - 1));

		auto mesh = std::make_shared<BasicMesh>();
		std::vector<BasicMesh::Vertex> vertices;

		if (propSeedMode.get() == 0) {
			auto indexBufferEuler = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
			auto indexBufferPoints = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
			// Draw start point
			vec2 startPoint = propStartPoint.get();
			vertices.push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
				vec3(0), vec3(0), vec4(0, 0, 0, 1) });
			indexBufferEuler->add(static_cast<std::uint32_t>(0));
			indexBufferPoints->add(static_cast<std::uint32_t>(0));
			// TODO: Create one stream line from the given start point

			//Hara: Define stepsize depending on whether we are integrating forward or backwards.
			float stepsize;
			if (propDirection.get() == 0) { //Forward direction
				stepsize = propStepSize.get();
			}
			else { //Backward direction
				stepsize = -1 * propStepSize.get();
			}

			//Jane: Using RK4 method to draw stream lines
			vec2 currentPoint_RK4 = startPoint;

			//Jane: if postion is outside of domain, ampleFromField function will return vec2[0,0]
			//      In the meantime, if vector is [0,0], integration should stop
			vec2 StopPoint = vec2(0, 0);

			vec2 priorPoint = currentPoint_RK4; // Needed to calculate length.
			double totalArcLength = 0; // Starts at 0.
			float maxArcLength = propmax.get(); // maximum cumulitive arc length
			float minArcLength = propmin.get(); // too slow velocity, minimum segment length

			for (int j = 0; j < numberOfSteps.get(); j++) {
				currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, stepsize, propfield.get());
				totalArcLength += Integrator::arclength(priorPoint, currentPoint_RK4); //Call the arclength function to get the length of the arc.
				if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
				if (totalArcLength > maxArcLength) break; //break loop if the arc exceeds maximum length.
				priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

											   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
											   //      as well as at the point where vector is [0,0]
				if (currentPoint_RK4 == StopPoint) {
					break;
				}
				else {
					vertices.push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
						vec3(0), vec3(0), vec4(0, 0, 0, 1) });
					indexBufferEuler->add(static_cast<std::uint32_t>(j + 1));
					indexBufferPoints->add(static_cast<std::uint32_t>(j + 1));
				}
			}

		}
		else {
			// TODO: Seed multiple stream lines randomly
			if (propSeeding.get() == 0) {
				int buffercount = 0;

				//Create 2-D Dimensional Arrays of Vertex Buffers
				int seeds = nseeds.get();
				std::vector<BasicMesh::Vertex>** VertexArray = new std::vector<BasicMesh::Vertex>*[seeds];
				for (int i = 0; i < seeds; i++) {
					VertexArray[i] = new std::vector<BasicMesh::Vertex>[2];
				}

				//Populate SeedArray
				for (int i = 0; i < seeds; i++) {
					auto indexBufferEuler = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
					vec2 startPoint = vec2((rand() % dims.x) + 1, (rand() % dims.y) + 1);
					VertexArray[i][0].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
						vec3(0), vec3(0), vec4(0, 0, 0, 1) });
					indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
					buffercount++;

					//Jane: Using RK4 method to draw stream lines
					vec2 currentPoint_RK4 = startPoint;

					//Jane: if postion is outside of domain, ampleFromField function will return vec2[0,0]
					//      In the meantime, if vector is [0,0], integration should stop
					vec2 StopPoint = vec2(0, 0);

					vec2 priorPoint = currentPoint_RK4; // Needed to calculate length.
					double totalArcLength = 0; // Starts at 0.
					float maxArcLength = propmax.get(); // maximum cumulitive arc length
					float minArcLength = propmin.get(); // too slow velocity, minimum segment length

					for (int k = 0; k < numberOfSteps.get(); k++) {
						currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, propStepSize.get(), propfield.get());
						totalArcLength += Integrator::arclength(priorPoint, currentPoint_RK4); //Call the arclength function to get the length of the arc.
						if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
						if (totalArcLength > maxArcLength) break; //break loop if the arc exceeds maximum length.
						priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

													   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
													   //      as well as at the point where vector is [0,0]
						if (currentPoint_RK4 == StopPoint) {
							break;
						}
						else {
							VertexArray[i][0].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
								vec3(0), vec3(0), vec4(0, 0, 0, 1) });
							indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
							buffercount++;
						}
					}
					mesh->addVertices(VertexArray[i][0]);

					//Repeat Entire Code With Modifications to Handle Backward Propogation

					//Hara: Reset Variables
					currentPoint_RK4 = startPoint;
					priorPoint = currentPoint_RK4;
					totalArcLength = 0;

					//Hara: Start New Vertex at Startpoint
					indexBufferEuler = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
					VertexArray[i][1].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
						vec3(0), vec3(0), vec4(0, 0, 0, 1) });
					indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
					buffercount++;

					//Repeat propogation loop, notice -1 in Integrator::RK4 method to propogate backwards.
					for (int k = 0; k < numberOfSteps.get(); k++) {
						currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, (-1 * propStepSize.get()), propfield.get());

						totalArcLength += Integrator::arclength(priorPoint, currentPoint_RK4); //Call the arclength function to get the length of the arc.
						if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
						if (totalArcLength > maxArcLength) break; //break loop if the arc exceeds maximum length.
						priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

													   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
													   //      as well as at the point where vector is [0,0]
						if (currentPoint_RK4 == StopPoint) {
							break;
						}
						else {
							VertexArray[i][1].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
								vec3(0), vec3(0), vec4(0, 0, 0, 1) });
							indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
							buffercount++;
						}
					}
					mesh->addVertices(VertexArray[i][1]);
				}
			}

			// TODO: Seed multiple stream lines using a uniform grid
			else if (propSeeding.get() == 1) {
				int buffercount = 0;

				//Create 3-D Dimensional Arrays of Vertex Buffers
				int seedsx = gridx.get();
				int seedsy = gridy.get();
				std::vector<BasicMesh::Vertex>*** VertexArray = new std::vector<BasicMesh::Vertex>**[seedsx];
				for (int i = 0; i < seedsx; i++) {
					VertexArray[i] = new std::vector<BasicMesh::Vertex>*[seedsy];
					for (int j = 0; j < seedsy; j++) {
						VertexArray[i][j] = new std::vector<BasicMesh::Vertex>[2]; //We want two arrays here, one for backwards and another for forwards
					}
				}

				//Populate SeedArray
				for (int i = 0; i < seedsx; i++) {
					for (int j = 0; j < seedsy; j++) {
						auto indexBufferEuler = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
						vec2 startPoint = vec2(((i + 1)*(dims.x - 1)) / ((float)seedsx + 1), ((j + 1)*(dims.y - 1)) / ((float)seedsy + 1));
						VertexArray[i][j][0].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
							vec3(0), vec3(0), vec4(0, 0, 0, 1) });
						indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
						buffercount++;

						//Jane: Using RK4 method to draw stream lines
						vec2 currentPoint_RK4 = startPoint;

						//Jane: if postion is outside of domain, ampleFromField function will return vec2[0,0]
						//      In the meantime, if vector is [0,0], integration should stop
						vec2 StopPoint = vec2(0, 0);

						vec2 priorPoint = currentPoint_RK4; // Needed to calculate length.
						double totalArcLength = 0; // Starts at 0.
						float maxArcLength = propmax.get(); // maximum cumulitive arc length
						float minArcLength = propmin.get(); // too slow velocity, minimum segment length

						for (int k = 0; k < numberOfSteps.get(); k++) {
							currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, propStepSize.get(), propfield.get());
							totalArcLength += Integrator::arclength(priorPoint, currentPoint_RK4); //Call the arclength function to get the length of the arc.
							if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
							if (totalArcLength > maxArcLength) break; //break loop if the arc exceeds maximum length.
							priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

														   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
														   //      as well as at the point where vector is [0,0]
							if (currentPoint_RK4 == StopPoint) {
								break;
							}
							else {
								VertexArray[i][j][0].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
									vec3(0), vec3(0), vec4(0, 0, 0, 1) });
								indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
								buffercount++;
							}
						}
						mesh->addVertices(VertexArray[i][j][0]);

						//Repeat Entire Code With Modifications to Handle Backward Propogation

						//Hara: Reset Variables
						currentPoint_RK4 = startPoint;
						priorPoint = currentPoint_RK4;
						totalArcLength = 0;

						//Hara: Start New Vertex at Startpoint
						indexBufferEuler = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::Strip);
						VertexArray[i][j][1].push_back({ vec3(startPoint.x / (dims.x - 1), startPoint.y / (dims.y - 1), 0),
							vec3(0), vec3(0), vec4(0, 0, 0, 1) });
						indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
						buffercount++;

						//Repeat propogation loop, notice -1 in Integrator::RK4 method to propogate backwards.
						for (int k = 0; k < numberOfSteps.get(); k++) {
							currentPoint_RK4 = Integrator::RK4(vol.get(), currentPoint_RK4, (-1 * propStepSize.get()), propfield.get());
							totalArcLength += Integrator::arclength(priorPoint, currentPoint_RK4); //Call the arclength function to get the length of the arc.
							if (Integrator::arclength(priorPoint, currentPoint_RK4) < minArcLength) break;//break loop if the velocity is too slow
							if (totalArcLength > maxArcLength) break; //break loop if the arc exceeds maximum length.
							priorPoint = currentPoint_RK4; //Set the priorpoint for the next loop (remember the prior point of the current loop was the current point at the end of previous loop.

														   //Jane: Stop integration at the boundry of domain where currentPoint_RK4 == vec2[0,0]
														   //      as well as at the point where vector is [0,0]
							if (currentPoint_RK4 == StopPoint) {
								break;
							}
							else {
								VertexArray[i][j][1].push_back({ vec3(currentPoint_RK4.x / (dims.x - 1), currentPoint_RK4.y / (dims.y - 1), 0),
									vec3(0), vec3(0), vec4(0, 0, 0, 1) });
								indexBufferEuler->add(static_cast<std::uint32_t>(buffercount));
								buffercount++;
							}
						}
						mesh->addVertices(VertexArray[i][j][1]);
					}
				}

			}

		}

		mesh->addVertices(vertices);
		outMesh.setData(mesh);
	}

}  // namespace inviwo