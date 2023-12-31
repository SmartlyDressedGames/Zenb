# Zen Builder

![Screenshot of in-game castle](https://github.com/SmartlyDressedGames/Zenb/assets/12567645/d9a6e8dc-e636-4127-b134-50e29cc554c8)

Video overview: https://youtu.be/pLTfZGN1YSk

This is a small hobby project I was playing around with in November 2022. I love building with Lego in real life, but found digital Lego-style tools like stud.io less user-friendly than game engine editors. 

The code isn't *great* (e.g., binding inputs by key rather than actions), but I hope by sharing the files it might come in useful for someone. I don't have any current plans to continue this project.

Prior to the project I was impressed by Unreal's automatic mesh instancing, and was curious to see how well it could perform with thousands of individual actors without any manual/gameplay instancing code. The baseplate is an exception and does use an instanced mesh component so that huge 1000x1000 baseplates don't need 1,000,000 actual studs. It does actually hold up very well! I think you would need smarter optimization to make a game with this, however. Brickadia is also made in Unreal and has an interesting development blog with insights into their techniques: https://brickadia.com/blog/
