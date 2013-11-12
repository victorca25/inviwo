#ifndef IVW_MESHGLRENDERER_H
#define IVW_MESHGLRENDERER_H

#include <modules/opengl/geometry/geometrygl.h>
#include <inviwo/core/datastructures/geometry/attributes.h>
#include <modules/opengl/geometry/attributebuffergl.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/representationconverter.h>
#include <vector>

namespace inviwo {

class IVW_MODULE_OPENGL_API MeshGLRenderer {

public:
    MeshGLRenderer(const Mesh* mesh);
    MeshGLRenderer(const Mesh* mesh, Mesh::AttributesInfo);
    MeshGLRenderer(const Mesh* mesh, RenderType rt, ConnectivityType ct);
    virtual ~MeshGLRenderer();

    virtual void render(RenderType = NOT_SPECIFIED) const;


    GLenum getDrawMode(RenderType, ConnectivityType);

protected:

    virtual void initialize(Mesh::AttributesInfo = Mesh::AttributesInfo());
    void initializeIndexBuffer( const Buffer* indexBuffer, Mesh::AttributesInfo ai );
    void renderArray(RenderType) const;
    void renderElements(RenderType) const;
    void emptyFunc(RenderType rt) const{};

    typedef void (MeshGLRenderer::*DrawFunc)(RenderType) const;
    struct DrawMethod{
        DrawFunc drawFunc;
        GLenum drawMode;
        const Buffer* elementBuffer;
    };

    DrawMethod drawMethods_[NUMBER_OF_RENDER_TYPES];
    const Mesh* meshToRender_;
};

} // namespace

#endif // IVW_MESHGLRENDERER_H