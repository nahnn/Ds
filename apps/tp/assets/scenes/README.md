# Test scenes

The `assets/scenes/test_*.scn` files are opened with File > Open. Each one
starts with a comment describing what to observe in the editor and in the
ray tracer (play button).

| Scene | Feature |
|---|---|
| `test_pbr_grid` | metallic x roughness |
| `test_materials_types` | PBR dielectrics and metals, Phong converted to PBR |
| `test_default_material` | mesh without material |
| `test_lights_types` | point, directional and spot lights |
| `test_lights_falloff` | falloff and range |
| `test_no_lights` | default light of the editor |
| `test_raytracing` | reflections, shadows, Fresnel, Min Weight |
| `test_camera_parallel` | parallel projection |
| `test_sweep_spiral` | spiral sweeps |
| `test_sweep_torsion` | torsion sweeps |
| `test_primitives` | default meshes and OBJ mesh |
| `test_invalid_*` | parser errors (scene is not loaded) |

## Manual checklist

| Steps | Expected |
|---|---|
| File > New Scene | camera, directional light and a box |
| Create > 3D Object > each mesh; Create > Sweep Object > Spiral and Torsion | new object with a new PBR material |
| Inspector of a PBR object: drag Metallic and Roughness | editor and ray tracer follow the values |
| Inspector of a sweep: change Generator, Points, Segments, Angle, Scale, Rotation and every sweep slider | mesh is rebuilt at once |
| Inspector of a sweep: check Open Arc or Enable Extrusion | Capped is disabled |
| Hierarchy: right click a sweep > Duplicate; edit the copy | the original does not change |
| Inspector of a sweep: Mesh name with a space, `.` or `/`, then Save .OBJ | error popup, no file written |
| Inspector of a sweep: Mesh name empty or with `"` | name is not changed |
| Inspector of a sweep: valid Mesh name > Save .OBJ | file in `assets/meshes`, listed by the "..." button of a mesh after restarting the app |
| Open `test_sweep_spiral`, File > Save, open `test_roundtrip` | same objects, materials, lights and sweep attributes |
| Open `test_raytracing`, Ray Tracing > Min Weight 0.001, 0.05 and 0.3 | fewer mirror images and rays (console) as the weight grows |
| Ray Tracing > Max Recursion Level 0 and 10 | no reflections with 0 |
