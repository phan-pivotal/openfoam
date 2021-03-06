/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2013-2016 OpenFOAM Foundation
     \\/     M anipulation  | Copyright (C) 2015-2016 OpenCFD Ltd.
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

#include "removeRegisteredObject.H"
#include "Time.H"
#include "polyMesh.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(removeRegisteredObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        removeRegisteredObject,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::removeRegisteredObject::removeRegisteredObject
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    regionFunctionObject(name, runTime, dict),
    objectNames_()
{
    read(dict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::removeRegisteredObject::~removeRegisteredObject()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionObjects::removeRegisteredObject::read(const dictionary& dict)
{
    regionFunctionObject::read(dict);

    dict.lookup("objects") >> objectNames_;

    return true;
}


bool Foam::functionObjects::removeRegisteredObject::execute()
{
    forAll(objectNames_, i)
    {
        if (foundObject<regIOobject>(objectNames_[i]))
        {
            const regIOobject& obj =
                lookupObject<regIOobject>(objectNames_[i]);

            if (obj.ownedByRegistry())
            {
                Log << type() << " " << name() << " output:" << nl
                    << "    removing object " << obj.name() << nl
                    << endl;

                const_cast<regIOobject&>(obj).release();
                delete &obj;
            }
        }
    }

    return true;
}


bool Foam::functionObjects::removeRegisteredObject::write()
{
    return true;
}


// ************************************************************************* //
