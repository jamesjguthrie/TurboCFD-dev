/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "homogeneousMixture.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

template<class ThermoType>
const char* Foam::homogeneousMixture<ThermoType>::specieNames_[1] = {"b"};


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::homogeneousMixture<ThermoType>::homogeneousMixture
(
    const dictionary& thermoDict,
    const fvMesh& mesh
)
:
    basicMultiComponentMixture
    (
        thermoDict,
        speciesTable(nSpecies_, specieNames_),
        mesh
    ),

    reactants_(thermoDict.subDict("reactants")),
    products_(thermoDict.subDict("products")),
    mixture_("mixture", reactants_),
    b_(Y("b"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class ThermoType>
const ThermoType& Foam::homogeneousMixture<ThermoType>::mixture
(
    const gpuField<scalar> b
) const
{
    //figure out how to do calculations
    if (b > 0.999)
    {
        return reactants_;
    }
    else if (b < 0.001)
    {
        return products_;
    }
    else
    {
        mixture_ = b/reactants_.W()*reactants_;
        mixture_ += (1 - b)/products_.W()*products_;

        return mixture_;
    }
}


template<class ThermoType>
void Foam::homogeneousMixture<ThermoType>::read(const dictionary& thermoDict)
{
    reactants_ = ThermoType(thermoDict.subDict("reactants"));
    products_ = ThermoType(thermoDict.subDict("products"));
}


template<class ThermoType>
const ThermoType& Foam::homogeneousMixture<ThermoType>::getLocalThermo
(
    const label specieI
) const
{
    if (specieI == 0)
    {
        return reactants_;
    }
    else if (specieI == 1)
    {
        return products_;
    }
    else
    {
        FatalErrorIn
        (
            "const ThermoType& Foam::homogeneousMixture<ThermoType>::"
            "getLocalThermo"
            "("
                "const label "
            ") const"
        )   << "Unknown specie index " << specieI << ". Valid indices are 0..1"
            << abort(FatalError);

        return reactants_;
    }
}


// ************************************************************************* //
