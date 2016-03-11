//
// Copyright (c) 2016 CNRS
// Authors: Anna Seppala
//
// This file is part of hpp-core
// hpp-core is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-core is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-core  If not, see
// <http://www.gnu.org/licenses/>.

#include <hpp/affordance/affordance-extraction.hh>
#include <hpp/affordance/operations.hh>
#include<hpp/fcl/collision_object.h>
#include <hpp/fcl/collision.h>
#include <hpp/fcl/BVH/BVH_model.h>
#include <algorithm>


namespace hpp {
  namespace affordance {

    typedef fcl::BVHModel<fcl::OBBRSS> BVHModelOB;
    typedef boost::shared_ptr<const BVHModelOB> BVHModelOBConst_Ptr_t;

    BVHModelOBConst_Ptr_t GetModel (const fcl::CollisionObjectConstPtr_t object)
    {
        assert (object->collisionGeometry ()->getNodeType () == fcl::BV_OBBRSS);
        const BVHModelOBConst_Ptr_t model = boost::static_pointer_cast<const BVHModelOB>
                                            (object->collisionGeometry ());
        assert (model->getModelType () == fcl::BVH_MODEL_TRIANGLES);
        return model;
    }

    void searchLinkedTriangles(std::vector<unsigned int>& listPotential, const OperationBasePtr_t& refOp,
                               const std::vector<Triangle>& allTris, std::vector<unsigned int>& searchableTris,
                               const unsigned int& refTriIdx, double& area)
    {
      const float marginRad = 0.3;
      Triangle refTri = allTris[refTriIdx];
      // find a cleaner way of removing & resizing the searchableTriangels vector
      std::remove (searchableTris.begin (), searchableTris.end (), refTriIdx);
      searchableTris.pop_back ();
      for (unsigned int searchIdx = 0; searchIdx < allTris.size (); searchIdx++) {
        std::vector<unsigned int>::iterator it = std::find (searchableTris.begin (),
                                                            searchableTris.end (), searchIdx);
          if (it == searchableTris.end ()) {
            continue;
          }
        for (unsigned int vertIdx = 0; vertIdx < 3; vertIdx++) {
          Triangle searchTri = allTris [searchIdx];
          if ((*refTri.fclTri)[vertIdx] == (*searchTri.fclTri)[0]
              || (*refTri.fclTri)[vertIdx] == (*searchTri.fclTri)[3]
              || (*refTri.fclTri)[vertIdx] == (*searchTri.fclTri)[2]) {
            if (refOp->requirement (searchTri.normal)) {
              if ((searchTri.normal - refTri.normal).sqrLength () < marginRad) {
                area += searchTri.area;
                listPotential.push_back (searchIdx);
                searchLinkedTriangles (listPotential, refOp, allTris,
                                       searchableTris, searchIdx, area);
              }
            } else {
              // if linked face does not fulfil global requirement, discard
              std::remove (searchableTris.begin (), searchableTris.end (), searchIdx);
              searchableTris.pop_back ();
            }
          }
        }
      }
    }

    SemanticsDataPtr_t AffordanceAnalysis (const fcl::CollisionObjectPtr_t& colObj,
                                           const std::vector <OperationBasePtr_t> & opVec)
    {
      BVHModelOBConst_Ptr_t model =  GetModel (colObj);
      std::vector <Triangle> triangles;
      std::vector <unsigned int> unsetTriangles;
      double totArea = .0;
      std::vector<std::vector<unsigned int> > potentialAffordances (opVec.size ());
      SemanticsDataPtr_t foundAffordances(new SemanticsData(opVec.size ()));

      for(unsigned int i = 0; i < model->num_tris; ++i)
      {
        TrianglePoints tri;
        fcl::Triangle fcltri = model->tri_indices [i];
        tri.p1 = colObj->getRotation () *
          model->vertices [fcltri [0]] + colObj->getTranslation ();
        tri.p2 = colObj->getRotation () *
          model->vertices [fcltri [1]] + colObj->getTranslation ();
        tri.p3 = colObj->getRotation () *
          model->vertices [fcltri [2]] + colObj->getTranslation ();

        triangles.push_back (Triangle (fcltri, tri));
        // save vector index of triangles and their quantity.
	unsetTriangles.push_back(i);
      }
      std::vector <unsigned int> unseenTriangles;
      for (unsigned int triIdx = 0; triIdx < triangles.size (); triIdx++) {
        // look for triangle in set of triangles not yet given an affordance:
        std::vector<unsigned int>::iterator it = std::find (unsetTriangles.begin (), unsetTriangles.end (), triIdx);
        if (it == unsetTriangles.end ()) {
          continue;
        }
        // set list of searchable (unseen) triangles equal to all triangles not yet given an affordance.
        unseenTriangles = unsetTriangles;
        for (unsigned int opIdx = 0; opIdx < opVec.size (); opIdx++) {
          if (opVec[opIdx]->requirement (triangles[triIdx].normal)) {
             totArea += triangles[triIdx].area;
             potentialAffordances[opIdx].push_back(triIdx);
             searchLinkedTriangles(potentialAffordances [opIdx], opVec[opIdx], triangles, unseenTriangles, triIdx, totArea);
            if (totArea > opVec[opIdx]->minArea_) {
              // save totArea for further use as well?
              AffordancePtr_t aff(new Affordance (potentialAffordances [opIdx], colObj));
              foundAffordances->affordances_ [opIdx].push_back (aff);
              for (unsigned int removeIdx = 0; removeIdx < potentialAffordances [opIdx].size (); removeIdx++) {
                std::remove (unsetTriangles.begin (), unsetTriangles.end (), potentialAffordances [opIdx][removeIdx]);
                unsetTriangles.pop_back ();
              }
                potentialAffordances [opIdx].clear ();
             }
             break;
          }
        }
      }
      return foundAffordances;
    }

  } // namespace affordance
} // namespace hpp


