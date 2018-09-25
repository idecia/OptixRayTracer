#pragma once

#include "shapes/Parallelogram.h"
#include "materials/Matte.h"
#include "core/optix_global.h"

class MatteParallelogram {

public:

   RT_FUNCTION MatteParallelogram();
   RT_FUNCTION MatteParallelogram(Parallelogram* parallelogram, Matte* material);
   RT_FUNCTION_HOST GeometryInstance GetOptixGeometry(Context context);

private:

   Parallelogram* parallelogram;
   Matte* material;

};

RT_FUNCTION MatteParallelogram::MatteParallelogram() { }

RT_FUNCTION MatteParallelogram::MatteParallelogram(Parallelogram* parallelogram, Matte* material)
   : parallelogram(parallelogram), material(material) {


}

RT_FUNCTION_HOST GeometryInstance MatteParallelogram::GetOptixGeometry(Context context) {

   Geometry geometry = parallelogram->GetOptixGeometry(context);
   Material  matte = material->GetOptixMaterial(context);
   GeometryInstance optixGeometryInstance = context->createGeometryInstance(geometry, &matte, &matte + 1);
   return optixGeometryInstance;

}