#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/range/iterator_range.hpp>
#include "anki/resource/Model.h"
#include "anki/resource/Material.h"
#include "anki/resource/Mesh.h"
#include "anki/resource/SkelAnim.h"
#include "anki/resource/MeshData.h"
#include "anki/resource/Skeleton.h"



//==============================================================================
// load                                                                        =
//==============================================================================
void Model::load(const char* filename)
{
	try
	{
		//
		// Load
		//
		using namespace boost::property_tree;
		ptree pt_;
		read_xml(filename, pt_);

		const ptree& pt = pt_.get_child("model");

		// modelPatches
		BOOST_FOREACH(const ptree::value_type& v, pt.get_child("modelPatches"))
		{
			const std::string& mesh = v.second.get<std::string>("mesh");
			const std::string& material = v.second.get<std::string>("material");

			ModelPatch* patch = new ModelPatch(mesh.c_str(), material.c_str());
			modelPatches.push_back(patch);
		}

		if(modelPatches.size() < 1)
		{
			throw ANKI_EXCEPTION("Zero number of model patches");
		}

		// Bounding volume
		visibilityShape = modelPatches[0].getMesh().getVisibilityShape();
		BOOST_FOREACH(
			const ModelPatch& patch,
			boost::make_iterator_range(modelPatches.begin() + 1,
			modelPatches.end()))
		{
			visibilityShape = visibilityShape.getCompoundShape(
				patch.getMesh().getVisibilityShape());
		}
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Model \"" + filename + "\": " + e.what());
	}
}
