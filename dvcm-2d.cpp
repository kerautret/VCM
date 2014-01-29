/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file voronoimap2D.cpp
 * @ingroup Examples
 * @author David Coeurjolly (\c david.coeurjolly@liris.cnrs.fr )
 * Laboratoire d'InfoRmatique en Image et Systèmes d'information - LIRIS (CNRS, UMR 5205), CNRS, France
 *
 * @date 2013/01/17
 *
 * An example file named voronoimap2D.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"

//! [Voro2D-header]
#include "DGtal/kernel/BasicPointPredicates.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/images/imagesSetsUtils/SimpleThresholdForegroundPredicate.h"
#include "DGtal/geometry/volumes/distance/ExactPredicateLpSeparableMetric.h"
#include "DGtal/geometry/volumes/distance/VoronoiMap.h"
#include "DGtal/geometry/volumes/distance/DistanceTransformation.h"

#include "DGtal/io/colormaps/HueShadeColorMap.h"
#include "DGtal/io/boards/Board2D.h"
//! [Voro2D-header]

///////////////////////////////////////////////////////////////////////////////

using namespace std;

typedef DGtal::Z2i::Space Space;
typedef DGtal::Z2i::Vector Vector;
typedef DGtal::Z2i::Point Point;
typedef DGtal::HyperRectDomain<Space> Domain;
typedef DGtal::ImageContainerBySTLVector<Domain,bool> CharacteristicSet;
typedef DGtal::ExactPredicateLpSeparableMetric<Space, 2> Metric; // L2-metric

// Model of CPointPredicate
struct CharacteristicSetPredicate {
  typedef CharacteristicSetPredicate Self;
  typedef DGtal::Z2i::Point Point;
  CharacteristicSetPredicate() : ptrSet( 0 ) {}
  CharacteristicSetPredicate( const CharacteristicSet& aSet) : ptrSet( &aSet ) {}
  CharacteristicSetPredicate( const Self& other ) : ptrSet( other.ptrSet ) {}
  Self& operator=( const Self& other ) { ptrSet = other.ptrSet; return *this; }
  bool operator()( const Point& p ) const
  { 
    ASSERT( ptrSet != 0 );
    return (*ptrSet)( p );
  }
private:
  const CharacteristicSet* ptrSet;
};

using namespace DGtal;

///////////////////////////////////////////////////////////////////////////////

int main( int /*argc*/, char** /*argv*/ )
{

  Metric l2;

  Z2i::Point lower(0,0);
  Z2i::Point upper(16,16);
  Z2i::Domain domain(lower,upper);

  // Définit les points de X.
  CharacteristicSet charSet( domain );
  charSet.setValue( Point(2,3), true );
  charSet.setValue( Point(7,15), true );
  charSet.setValue( Point(12,5), true );
  charSet.setValue( Point(14,10), true );
  Board2D board;

  board<< domain;
  for ( Domain::ConstIterator it = domain.begin(), itE = domain.end();
        it != itE; ++it )
    if ( charSet( *it ) ) board << *it;
  board.saveSVG("voronoimap-inputset.svg");

  // Le diagramme de Voronoi est calculé sur le complément de X.
  CharacteristicSetPredicate inCharSet( charSet );
  typedef NotPointPredicate<CharacteristicSetPredicate> NotPredicate;
  NotPredicate notSetPred( inCharSet);

  trace.beginBlock ( "Calcul du diagramme de Voronoi 2D" );
  typedef VoronoiMap<Z2i::Space, NotPredicate, Metric > Voronoi2D;
  Voronoi2D voronoimap(domain,notSetPred,l2);
  trace.endBlock();

  // On affiche le vecteur vers le site le plus proche.
  board.clear();
  board << domain;
  for(Voronoi2D::Domain::ConstIterator it = voronoimap.domain().begin(),
      itend = voronoimap.domain().end(); it != itend; ++it)
  {
    Voronoi2D::Value site = voronoimap( *it );   //closest site to (*it)
    if (site != (*it))
      Display2DFactory::draw( board,   site - (*it), (*it)); //Draw an arrow
  }
  board.saveSVG("voronoimap-voro.svg");

  // On affiche le vecteur vers le site le plus proche seulement si il est à distance <= 4.
  board.clear();
  board << domain;
  for(Voronoi2D::Domain::ConstIterator it = voronoimap.domain().begin(),
      itend = voronoimap.domain().end(); it != itend; ++it)
  {
    Voronoi2D::Value site = voronoimap( *it );   //closest site to (*it)
    if (site != (*it))
      {
        double d = l2( site, *it );
        if( d <= 4.0 )
        { 
            Display2DFactory::draw( board,   site - (*it), (*it)); //Draw an arrow
        }
      }
  }
  board.saveSVG("voronoimap-voro-4.svg");

  return 0;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
