# Humanoid Path Planner - Affordance module

[![Pipeline status](https://gepgitlab.laas.fr/humanoid-path-planner/hpp-affordance/badges/master/pipeline.svg)](https://gepgitlab.laas.fr/humanoid-path-planner/hpp-affordance/commits/master)
[![Coverage report](https://gepgitlab.laas.fr/humanoid-path-planner/hpp-affordance/badges/master/coverage.svg?job=doc-coverage)](http://projects.laas.fr/gepetto/doc/humanoid-path-planner/hpp-affordance/master/coverage/)

Copyright 2016 LAAS-CNRS

Author: Anna Seppala

## Description

HPP - AFFORDANCE is a library that builds representations of a robot's environment in terms
of affordances, which describe surrounding objects based on their application possibilities.
For instance, large vertical surfaces may be applied for leaning, whereas smaller horizontal
surface areas (e.g. stairs) allow for a robot to step on them.
This idea was presented in multiple papers by Kaiser et al. [(link)] (http://h2t.anthropomatik.kit.edu/english/21_459.php):

"Extracting Whole-Body Affordances from Multimodal Exploration" (IEEE/RAS 2014)

"Extraction of Whole-Body Affordances for Loco-Manipulation Tasks" (IJHR 2015)

"Validation of Whole-Body Loco-Manipulation Affordances for Pushability and Liftability" (IEEE/RAS 2015)

The HPP - AFFORDANCE module is loosely based on the methods of affordance extraction presented
above but analyses triangulated mesh files based on triangle normals.

This library is part of the software Humanoid Path Planner [(link)](http://projects.laas.fr/gepetto/index.php/Software/Hpp).

## Installation

### from robotpkg apt binaries

  1. Follow "Binary packages in robotpkg/wip" instructions: http://robotpkg.openrobots.org/robotpkg-wip.html
  2  `apt install robotpkg-hpp-affordance`

### from sources

To install HPP - AFFORDANCE, you will need to install one other package of the Humanoid Path Planner software with its respective dependecies. Please see the instructions below for the full installation of HPP - AFFORDANCE:

  1. Install HPP - FCL
	- see https://github.com/humanoid-path-planner/hpp-fcl

  2. Install Eigen 3
	- see http://eigen.tuxfamily.org/

  3. Clone the HPP - AFFORDANCE repository onto your local computer and update the submodule:

			git clone --recursive https://github.com/anna-seppala/hpp-affordance.git
			cd hpp-affordance

  4. Use CMake to install the HPP - AFFORDANCE library. For instance:

			mkdir build
			cd build
			cmake ..
			make install


## Documentation

Open $DEVEL_DIR/install/share/doc/hpp-affordance/doxygen-html/index.html or
http://projects.laas.fr/gepetto/doc/humanoid-path-planner/hpp-affordance/master/doxygen-html/ in a web brower and you
will have access to the code documentation.

