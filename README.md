# ProceduralHouseGeneration

Description:
For my master project, I developed a novel techinique for the procedural generation of suburban houses with integrated floor plan, 
with the goal of overcoming some limitations found in the existing public techniques.

Features:
Anlysing previous researches, It was noticed how some of the techinques for the generation of suburnan houses:
- generate the floor plan of the house inside a fixed external shape, limiting the variety of the results
- build the 3d model of the house in the algorithm itself, preventing the user from using meshes they designed to personalise the look of the house

Therefore, the goal of the project was to develop a new method to generate such architecures, offering the following features:
- Generate houses with more various floor plans, dictaded by internal rooms disposition
- Offer the user the possibility to use external modular meshes to build the final model of the house
- Let the user define constraints such as the rooms in the house, their minimum area and their connections

Results
The algorithm was implemented in Unreal Engine to evaluate the obtainable results and test its use in a game development scenario.

The results showed how the technique can generate houses with more diverse floor plans, 
which differ both for their external shape and for the shape of each room. Furthermore, the techinque respects both the contraints given by the user and guidelines used in architectural design. Lastly, the algorithm can correctly position the meshes provided by the user, automating a task which is normally given to a design team.

The implementation is however not complete, there are few edge cases that procude unintended results and some components 
such as the roof and multple floors need to be added.

Nevertheless, the method demostrates to be a good alternative for the procedural generation of suburban houses.
Possibly, its development will be carried on after the conclusion of the master, with then intention of creating a tool for game developers
