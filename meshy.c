#include <polymec/polymec.h>

// function to create a cube mesh
void add_simple_3x3x3_mesh(silo_file_t* silo)
{
  // Let's make the simplest damn mesh possible.
  int nx = 3, ny = 3, nz = 3; // number of cells in each direction

  bbox_t bbox = {.x1 = 0.0, .x2 = 1.0,  // <-- physical bounding coordinates
                 .y1 = 0.0, .y2 = 1.0,
                 .z1 = 0.0, .z2 = 1.0};

  // Make the mesh. mesh_t* is a pointer to a mesh object. simple is the
  // variable that stores the pointer. it's the specific instance.
  // it's the mesh object.
  // mesh_t* is a type, like int, not an instance, like 10
  mesh_t* simple = create_uniform_mesh(MPI_COMM_WORLD, nx, ny, nz, &bbox);

  // Add the mesh to the file.
  silo_file_write_mesh(silo, "simple", simple);

  // Clean up.
  mesh_free(simple);
}

// function to create a cubed cylinder mesh
void add_cubed_c_mesh(silo_file_t* silo)
{
  // Make the mesh with the parameters specified in the geometry file.
  mesh_t* cubed_c = create_cubed_cylinder_mesh(MPI_COMM_WORLD,
                                   5, 5, 1.0, 1.0, 0.5, false, 
                                   "R", "bottom", "top");


  // Add the mesh to the file.
  silo_file_write_mesh(silo, "cubed_c", cubed_c);

  // Clean up.
  mesh_free(cubed_c);
}


 
// function to create a cylinder mesh
 void add_cropped_c_mesh(silo_file_t* silo) // cylinder is my mesh object name
{
  // Create a cubic uniform mesh.
  int Nx = 10, Ny = 10, Nz = 5;
  real_t height = 0.2;
  real_t dz = height/Nz;
  bbox_t bbox = {.x1 = 0.0, .x2 = 1.0, .y1 = 0.0, .y2 = 1.0, .z1 = -dz, .z2 = height+dz};
  mesh_t* mesh = create_uniform_mesh(MPI_COMM_WORLD, Nx, Ny, Nz, &bbox);

  // Create a cropped mesh using a cylinder.
  mesh_t* almost_cropped_mesh;
  {
    point_t O = {.x = 0.5, .y = 0.5, .z = 0.5};
    sp_func_t* cyl = cylinder_new(&O, 0.5, INWARD_NORMAL);
    vector_t ntop = {.x = 0.0, .y = 0.0, .z = -1.0}; 
    point_t xtop = {.x = 0.0, .y = 0.0, .z = height + dz}; // top plane
    vector_t nbot = {.x = 0.0, .y = 0.0, .z = 1.0}; 
    point_t xbot = {.x = 0.0, .y = 0.0, .z = 0.0 - dz}; // bottom plane
    sp_func_t* ptop = plane_new(&ntop, &xtop);
    sp_func_t* pbot = plane_new(&nbot, &xbot);
    sp_func_t* surfaces[] = {cyl, ptop, pbot};
    sp_func_t* boundary = intersection_new(surfaces, 3);
    almost_cropped_mesh = crop_mesh(mesh, boundary, PROJECT_NODES);
    mesh_free(mesh);
  }

  // Projecting the nodes *almost* works like it should, but there are some 
  // artifacts. We chop off the top and bottom now to get rid of these.
  mesh_t* cropped_mesh;
  {
    vector_t ntop = {.x = 0.0, .y = 0.0, .z = -1.0}; 
    point_t xtop = {.x = 0.0, .y = 0.0, .z = height};
    vector_t nbot = {.x = 0.0, .y = 0.0, .z = 1.0}; 
    point_t xbot = {.x = 0.0, .y = 0.0, .z = 0.0};
    sp_func_t* ptop = plane_new(&ntop, &xtop);
    sp_func_t* pbot = plane_new(&nbot, &xbot);
    sp_func_t* surfaces[] = {ptop, pbot};
    sp_func_t* boundary = intersection_new(surfaces, 2);
    cropped_mesh = crop_mesh(almost_cropped_mesh, boundary, REMOVE_CELLS);
    mesh_free(almost_cropped_mesh);
  }

  // Plot the cropped mesh.
  double ones[Nx*Ny*Nz];
  for (int c = 0; c < Nx*Ny*Nz; ++c)
    ones[c] = 1.0*c;
  silo_file_write_mesh(silo, "cropped_c", cropped_mesh);
  silo_file_write_scalar_cell_field(silo, "solution", "cropped_c", ones, NULL);

  mesh_free(cropped_mesh);
}


int main(int argc, char* argv[])
{
  // Initialize polymec.
  polymec_init(argc, argv);

  // Now create a SILO file for visualization.
  silo_file_t* silo = silo_file_new(MPI_COMM_WORLD, "meshy", ".", 1, 0, 0, 0.0);

  // Add meshes.
  add_simple_3x3x3_mesh(silo);
  add_cubed_c_mesh(silo);
  add_cropped_c_mesh(silo);

  // Write the SILO file.
  silo_file_close(silo);

  return 0;
}
