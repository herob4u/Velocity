#include "vctPCH.h"
#include "Model.h"

#include "Engine/Renderer/Texture/Texture2D.h"
#include "Engine/Utils/StringUtils.h"

// Model importing
#include "tinyOBJ.h"
#include "ofbx.h"

using namespace Vct;

static const std::string FBX_FORMAT = "fbx";
static const std::string OBJ_FORMAT = "obj";

enum class ModelFormat : uint8_t
{
    FBX,
    OBJ,
    UNKNOWN
};

static ModelFormat GetModelFormat(const Path& resPath)
{
    std::string ext(3,0);
    resPath.GetExtension(ext);

    if(String::Equals(ext, FBX_FORMAT, false))
    {
        return ModelFormat::FBX;
    }
    else if(String::Equals(ext, OBJ_FORMAT, false))
    {
        return ModelFormat::OBJ;
    }
    
    return ModelFormat::UNKNOWN;
}

Model* Model::Import(const char * filepath)
{
    return nullptr;
}

Model* Model::FromMesh(Mesh* mesh, MaterialInstance* material)
{
    Model* model = new Model(StringId::NONE);
    model->m_Meshes.emplace_back(mesh);
    model->m_Materials.emplace_back(material);

    return model;
}

Model* Model::FromMeshes(const std::vector<Mesh*>& meshes, const std::vector<MaterialInstance*>& materials)
{
    Model* model = new Model(StringId::NONE);
    ASSERT(meshes.size() == materials.size(), "Mesh and material IDs must associate");

    for(int i = 0; i < meshes.size(); i++)
    {
        model->m_Meshes.push_back(std::unique_ptr<Mesh>(meshes[i]));
        model->m_Materials.push_back(materials[i]);
    }

    return model;
}

Model* Model::FromMeshes(std::vector<std::unique_ptr<Mesh>>& meshes, std::vector<MaterialInstance*>& materials)
{
    Model* model = new Model(StringId::NONE);
    model->m_Meshes     = std::move(meshes);
    model->m_Materials  = std::move(materials);

    return model;
}

Model::Model(const std::string& resFile)
    : Resource(resFile)
{
}

Model::Model(const Path& resPath)
    : Resource(resPath)
{
}

void Model::SetMaterial(uint8_t materialId, MaterialInstance* material)
{
    if(materialId >= m_Materials.size())
        m_Materials.resize(materialId + 1);

    m_Materials[materialId] = material;
}

Mesh* Model::GetMesh(uint8_t meshId) const
{
    if(meshId >= m_Meshes.size())
        return nullptr;

    return m_Meshes[meshId].get();
}

MaterialInstance* Model::GetMaterial(uint8_t materialId) const
{
    if(materialId >= m_Materials.size())
        return nullptr;

    return m_Materials[materialId];
}

bool Model::Load(const void* rawBinary, size_t bytes)
{
    ModelFormat modelFormat = GetModelFormat(GetPath());

    switch(modelFormat)
    {
        case ModelFormat::OBJ: return LoadOBJ(rawBinary, bytes);
        case ModelFormat::FBX: return LoadFBX(rawBinary, bytes);
        case ModelFormat::UNKNOWN: return false;

        default: {ASSERT(false, "Unexpected model format"); return false;}
    }
}

void Model::Unload()
{
}

void Model::UpdateDependencies()
{
    ResourceState state = GetState();

    if(state == ResourceState::LOADED)
    {
        //NotifyDependencyLoad(m_Texture);
    }
    else if(state == ResourceState::UNLOADED)
    {
        //NotifyDependencyUnload(m_Texture);
    }
}

static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
static std::ostream& operator<<(std::ostream& os, const glm::vec2& vec)
{
    os << "(" << vec.x << ", " << vec.y << ", " <<  ")";
    return os;
}

bool Model::LoadFBX(const void* rawBinary, size_t bytes)
{
    ofbx::u64 loadFlags = static_cast<ofbx::u64>(ofbx::LoadFlags::IGNORE_BLEND_SHAPES) | (ofbx::u64)ofbx::LoadFlags::TRIANGULATE;
    ofbx::IScene* scene = ofbx::load((const ofbx::u8*)rawBinary, (int)bytes, loadFlags);

    if(scene)
    {
        int meshCount = scene->getMeshCount();

        for(int i = 0; i < meshCount; i++)
        {
            m_Meshes.resize(meshCount);
            m_Materials.resize(meshCount);

            const ofbx::Mesh* mesh = scene->getMesh(i);
            const ofbx::Geometry* geom = mesh->getGeometry();

            int numVertices = geom->getVertexCount();
            int numIndices  = geom->getIndexCount();

            std::vector<MeshVertex> vertices;
            std::vector<Triangle> triangles;
            vertices.reserve(numVertices);
            triangles.reserve(numIndices/3);

            const ofbx::Vec3* fbxVertices   = geom->getVertices();
            const ofbx::Vec3* fbxNormals    = geom->getNormals();
            const ofbx::Vec2* fbxTexCoords  = geom->getUVs();
            const ofbx::Vec3* fbxTangents   = geom->getTangents();

            // Handle 3 indices at a time (triangle)
            for(int i = 0; i < numIndices; i += 3)
            {
                Triangle tri;
                // For each index (vertex)
                for(int v = 0; v < 3; v++)
                {
                    int idx = geom->getFaceIndices()[i+v];
                    if(idx < 0)
                    {
                        ASSERT(v == 2, "Expected end of triangle");
                        idx = (-1*idx -1);
                    }

                    MeshVertex vertex;
                    vertex.Position = glm::vec3(fbxVertices[idx].x, fbxVertices[idx].y, fbxVertices[idx].z);
                    vertex.Normal   = glm::vec3(fbxNormals[idx].x, fbxNormals[idx].y, fbxNormals[idx].z);
                    vertex.UV = glm::vec3(0.f);
                    vertex.Tangent = glm::vec3(0.f);
                    vertex.BiTangent = glm::vec3(0.f);

                    //VCT_WARN("MeshVertex(idx={0}):\n Pos: {1} \n Norm: {2}\n\n", idx, vertex.Position, vertex.Normal);

                    vertices.push_back(vertex);
                    tri.Idx[v] = i+v;
                }
                //VCT_WARN("Triangle: ({0}, {1}, {2})", tri.Idx[0], tri.Idx[1], tri.Idx[2]);
                triangles.push_back(tri);
            }

            /*
            for(int v = 0; v < numVertices; v++)
            {
                MeshVertex vertex;

                vertex.Position     = glm::vec3(fbxVertices[v].x, fbxVertices[v].y, fbxVertices[v].z);
                vertex.Normal       = glm::vec3(fbxNormals[v].x, fbxNormals[v].y, fbxNormals[v].z);
                vertex.UV           = glm::vec2(fbxTexCoords[v].x, fbxTexCoords[v].y);
                vertex.Tangent      = glm::vec3(fbxTangents[v].x, fbxTangents[v].y, fbxTangents[v].z);
                vertex.BiTangent    = glm::vec3(0.f);

                vertices.push_back(vertex);
            }
            
            // Important Assumption: 
            const int* fbxIndices = geom->getFaceIndices();
            for(int i = 0; i < numIndices; i += 3)
            {
                // The 3rd index is negated because FBX uses a negative sign on an index that
                // resembles the end of a polygon. The actual index is (-1 * idx) -1
                triangles.emplace_back(fbxIndices[i], fbxIndices[i+1], -fbxIndices[i+2]-1);
            }
            */
            m_Meshes[i] = (std::make_unique<StaticMesh>(vertices, triangles));
        }

        return true;
    }

    return false;
}

bool Model::LoadOBJ(const void* rawBinary, size_t bytes)
{
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;

    if(reader.ParseFromString((const char*)rawBinary, "", config))
    {
        // Fetch meshes
        std::vector<MeshVertex> vertices;
        std::vector<Triangle> triangles;

        const tinyobj::attrib_t& attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

        m_Meshes.resize(shapes.size());
        m_Materials.resize(shapes.size());

        // Loop over shapes; in our case, each shape corresponds to a mesh object
        for(size_t s = 0; s < shapes.size(); s++)
        {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                // Num of face vertices for face f
                int fv = shapes[s].mesh.num_face_vertices[f];
                ASSERT(fv == 3, "Only supporting triangles for now");

                Triangle tri;

                // Loop over vertices in the face.
                for(size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    tinyobj::real_t vx = 0.f, vy = 0.f, vz = 0.f;
                    tinyobj::real_t nx = 0.f, ny = 0.f, nz = 0.f;
                    tinyobj::real_t tx = 0.f, ty = 0.f;

                    vx = attrib.vertices[3 * idx.vertex_index + 0];
                    vy = attrib.vertices[3 * idx.vertex_index + 1];
                    vz = attrib.vertices[3 * idx.vertex_index + 2];

                    if(attrib.normals.size())
                    {
                        nx = attrib.normals[3 * idx.normal_index + 0];
                        ny = attrib.normals[3 * idx.normal_index + 1];
                        nz = attrib.normals[3 * idx.normal_index + 2];
                    }

                    if(attrib.texcoords.size())
                    {
                        tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                        ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    }

                    MeshVertex meshVertex;
                    meshVertex.Position = glm::vec3(vx, vy, vz);
                    meshVertex.Normal = glm::vec3(nx, ny, nz);
                    meshVertex.UV = glm::vec2(tx, ty);
                    meshVertex.BiTangent = glm::vec3(0.f);
                    meshVertex.Tangent = glm::vec3(0.f);

                    vertices.push_back(meshVertex);
                    tri.Idx[v] = static_cast<uint32_t>(index_offset + v);
                }
                triangles.emplace_back(tri);
                index_offset += fv;

                // per-face material
                //shapes[s].mesh.material_ids[f];
            }

            // Adding meshes should occur here!
            m_Meshes[s] = std::make_unique<StaticMesh>(vertices, triangles);
            // m_Materials[s] = ....
        }

        return true;
    }

    return false;
}