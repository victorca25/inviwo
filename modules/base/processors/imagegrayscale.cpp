#include "imagegrayscale.h"

namespace inviwo {

ImageGrayscale::ImageGrayscale()
    : ProcessorGL(),
      inport0_(Port::INPORT, "inport0"),
      outport_(Port::OUTPORT, "outport")
      
{
    addPort(inport0_);
    addPort(outport_);
}

ImageGrayscale::~ImageGrayscale() {}

Processor* ImageGrayscale::create() const {
    return new ImageGrayscale();
}

void ImageGrayscale::initialize() {
    ProcessorGL::initialize();
    shader_ = new Shader("img_graysc.frag");
}

void ImageGrayscale::deinitialize() {
    delete shader_;
    Processor::deinitialize();
}

void ImageGrayscale::process() {    
    
    Image* outImage = outport_.getData();
    ImageGL* outImageGL = outImage->getRepresentation<ImageGL>();
    ivec2 csize = outImageGL->dimension();

    activateTarget(outport_);
    bindColorTexture(inport0_, GL_TEXTURE0);

    shader_->activate();
    shader_->setUniform("inport0_", 0);
    shader_->setUniform("dimension_", vec2(1.f / csize[0], 1.f / csize[1]) );
    renderImagePlaneQuad();
    shader_->deactivate();

    deactivateCurrentTarget();
}

} // namespace
