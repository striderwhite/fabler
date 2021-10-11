# Fabler

Internal DLL "hack" for Fable: TLC

- Inf health / mana
- Ability to "jump" which was not in the original game
- Partially working ESP -- Just missing the ViewModel-Projection matrix address - once this is found esp will work

### Info
- Uses two hooks: hooking the D3D9 endScene function using a "Vtable Hook" method (overriding the vtable pointer to our own injected function) in order to draw primative objects on the screen using the direct 3d library.
- One "Injection Copy" style hook for reading address for "CTCPhysicsControlled" objects -- this class contained the XYZ of coordinates of NPCs and Enemies. This drives part of the data needed for the ESP calculation


### Cheat Table
Has a cheat table with lots of nice pointers to various useful objects during the reversing

### Reclass
Plenty of reversed classes in the reclass file

______________

This project took about 32 hours. The majority of it was RE efforts. The only missing step is finding the ViewModel-Projection matrix to properly draw the ESP. 