// -*- c-basic-offset: 4 -*-
/**  @file CleanCP.h
 *
 *  @brief algorithms for remove control points by statistic method
 *  
 *  the algorithm is based on ptoclean by Bruno Postle
 *
 *  @author Thomas Modes
 *
 *  $Id$
 *
 */
 
 /*  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _CLEANCP_H
#define _CLEANCP_H

#include <algorithm/PanoramaAlgorithm.h>
#include <panodata/Panorama.h>
#include <algorithms/optimizer/PTOptimizer.h>
#include "algorithms/basic/CalculateCPStatistics.h"

namespace HuginBase {

/** optimises images pairwise and removes for every image pair control points with error > mean+n*sigma 
  @param pano panorama which should be used
  @param n determines, how big the deviation from mean should be to determine wrong control points, default 2.0
  @return set which contains control points with error > mean+n*sigma */
UIntSet getCPoutsideLimit_pair(Panorama pano, double n=2.0);
/** optimises the whole panorama and removes all control points with error > mean+n*sigma 
  @param pano panorama which should be used
  @param n determines, how big the deviation from mean should be to determine wrong control points, default 2.0
  @return set which contains control points with error > mean+n*sigma */
UIntSet getCPoutsideLimit(Panorama pano, double n=2.0);

}  // namespace
#endif // _H