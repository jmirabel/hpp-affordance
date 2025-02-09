//
// Copyright (c) 2016 CNRS
// Authors: Anna Seppala
//
// This file is part of hpp-affordance
// hpp-affordance is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-affordance is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-affordance  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef HPP_AFFORDANCE_OPERATIONS_HH
#define HPP_AFFORDANCE_OPERATIONS_HH

# include <hpp/fcl/math/vec_3f.h>

namespace hpp {
  namespace affordance {
    /// \addtogroup affordance
    /// \{
		
		/// Base class for the data needed to determine affordances of different
		/// types. For each affordance type, a child class inheriting OperationBase
		/// will have to be created.
    class OperationBase
    {
      public:
				/// If an OperationBase object is created without parameters, an
				/// affordance type called "noAffordance" is created as default,
				/// with certain default values for its member variables. This
				/// constructor should not be used..!
        OperationBase (): zWorld_(0,0,1), margin_(0.3), neighbouringTriangleMargin_(0.3), 
                          minArea_(0.05), affordance_("noAffordance") {}
				/// Constructor that allows for user-defined parameters. Default values are given for
				/// parameters that are not defined by the user.
				/// \param margin Margin needed for the evaluation of the requirement function
				/// \param nbTriMargin Margin between two triangles tested for a single
        ///  affordance surface. If the angle between two triangles is greater
        ///  than the provided margin, the triangles cannot be part of the same
        ///  affordance surface.
        /// \param minArea Minimum area needed for the formation of an affordance object
				/// \param affordanceName The name of the affordance type
        explicit OperationBase (const double margin = 0.3, const double nbTriMargin = 0.3,
																const double minArea = 0.05,
                                const char* affordanceName = "noAffordance"):
																zWorld_(0,0,1), margin_(margin),
																neighbouringTriangleMargin_(nbTriMargin),
																minArea_(minArea), affordance_(affordanceName) {}
				/// Fully virtual function that will determine whether or not a given 
				/// triangle normal fullfils the requirement of a child class. If yes,
				/// the tested triangle forms part of a potential affordance.
				/// \param normal Normal vector of the tested triangle.
        virtual bool requirement (const fcl::Vec3f& normal) =0;
				/// The orientation of the world z axis. Needed to find potential affordance objects.
        const fcl::Vec3f zWorld_;
				/// The error margin within which the requirement function must be fullfilled.
        const double margin_;
				/// The margin for the deviation of the normal of neighbouring triangles.
				/// Used to determine affordance objects comprising more than one triangle.
				const double neighbouringTriangleMargin_;
				/// The minimum area required for an affordance object. The total area may
				/// comprise multiple triangles.
        const double minArea_;
				/// Name of the affordance type for which te requirement exists.
        const char* affordance_;
    }; // class OperationBase
		/// Class that contains the information needed to create affordance
		/// objects of type Support. Inherits the OperationBase class.
    class SupportOperation : public OperationBase
    {
      public:
				/// Constructor that takes in user-defined parameters
				/// \param margin Margin needed for the evaluation of the requirement function
				/// \param nbTriMargin Margin between two triangles tested for a single
        ///  affordance surface. If the angle between two triangles is greater
        ///  than the provided margin, the triangles cannot be part of the same
        ///  affordance surface.
				/// \param minArea Minimum area needed for the formation of an affordance object
				/// \param affordanceName The name of the affordance type
				explicit SupportOperation (const double margin = 0.3, const double nbTriMargin = 0.3,
																		const double minArea = 0.05, 
                                    const char* affordanceName = "Support"):
                                    OperationBase(margin, nbTriMargin, minArea, affordanceName) {}
				/// The implementation of the requirement function for Support affordances
				/// overrides the virtual function in class OperationBase.
				/// \param nromal Normal vector of the tested triangle.
        bool requirement (const fcl::Vec3f& normal) 
      {
        return ((zWorld_ - normal).squaredNorm() < margin_);
      }
    }; // class SupportOperation
    /// Class that contains the information needed to create affordance
		/// objects of type Lean. Inherits the OperationBase class.
    class LeanOperation : public OperationBase
    {
      public:
			/// Constructor that takes in user-defined parameters
			/// \param margin Margin needed for the evaluation of the requirement function
			/// \param nbTriMargin Margin between two triangles tested for a single
      ///  affordance surface. If the angle between two triangles is greater
      ///  than the provided margin, the triangles cannot be part of the same
      ///  affordance surface.
			/// \param minArea Minimum area needed for the formation of an affordance object
			/// \param affordanceName The name of the affordance type
      explicit LeanOperation (const double margin = 0.3, const double nbTriMargin = 0.3,
																const double minArea = 0.05,
                                const char* affordanceName = "Lean"):
                                OperationBase(margin, nbTriMargin, minArea, affordanceName) {}
				/// The implementation of the requirement function for Lean affordances
				/// overrides the virtual function in class OperationBase.
				/// \param nromal Normal vector of the tested triangle.
        bool requirement (const fcl::Vec3f& normal) 
        {
          return (fabs (normal.dot(zWorld_)) < margin_);
        }
    }; // class LeanOperation

    /// Class that contains the information needed to create affordance
        /// objects of type Lean. Inherits the OperationBase class.
    class Support45Operation : public OperationBase
    {
      public:
            /// Constructor that takes in user-defined parameters
            /// \param margin Margin needed for the evaluation of the requirement function
            /// \param nbTriMargin Margin between two triangles tested for a single
      ///  affordance surface. If the angle between two triangles is greater
      ///  than the provided margin, the triangles cannot be part of the same
      ///  affordance surface.
            /// \param minArea Minimum area needed for the formation of an affordance object
            /// \param affordanceName The name of the affordance type
      explicit Support45Operation (const double margin = 0.3, const double nbTriMargin = 0.3,
                                                                const double minArea = 0.05,
                                const char* affordanceName = "Support45"):
                                OperationBase(margin, 0.05, minArea, affordanceName)
                                , axis45_ (fcl::Vec3f(1./sqrt(2.),0,1./sqrt(2.))) {}
       private:
        const fcl::Vec3f axis45_;

                /// The implementation of the requirement function for Support45 affordances
                /// overrides the virtual function in class OperationBase.
                /// Test if the normal in oriented with 45° up +- margin
                /// \param nromal Normal vector of the tested triangle.
        bool requirement (const fcl::Vec3f& normal)
        {
          fcl::Vec3f projectedNormal(0,0,normal[2]); // project normal in x,z plan
          projectedNormal[0] = sqrt(normal[0]*normal[0] + normal[1]*normal[1]);
          return ((axis45_ - projectedNormal).squaredNorm() < margin_);
        }
    }; // class LeanOperation


		/// \} 
  } // namespace affordance
} // namespace hpp

#endif // HPP_AFFORDANCE_OPERATIONS_HH
