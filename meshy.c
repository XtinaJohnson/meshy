#include <polymec/polymec.h>

void add_simple_3x3x3_mesh(silo_file_t* silo)
{
  // Let's make the simplest damn mesh possible.
  int nx = 3, ny = 3, nz = 3;
  bbox_t bbox = {.x1 = 0.0, .x2 = 1.0,  // <-- physical bounding coordinates
                 .y1 = 0.0, .y2 = 1.0,
                 .z1 = 0.0, .z2 = 1.0};
  mesh_t* simple = create_uniform_mesh(MPI_COMM_WORLD, nx, ny, nz, &bbox);

  // Add the mesh to the file.
  silo_file_write_mesh(silo, "simple", simple);

  // Clean up.
  mesh_free(simple);
}

void add_cubed_c_mesh(silo_file_t* silo)
{
  mesh_t* cubed_c = create_cubed_cylinder_mesh(MPI_COMM_WORLD,
                                   5, 5, 1.0, 1.0, 0.5, false, 
                                   "R", "bottom", "top");


  // Add the mesh to the file.
  silo_file_write_mesh(silo, "cubed_c", cubed_c);

  // Clean up.
  mesh_free(cubed_c);
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

  // Write the SILO file.
  silo_file_close(silo);

  return 0;
}
