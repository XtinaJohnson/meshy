#include <polymec/polymec.h>

int main(int argc, char* argv[])
{
  // Let's make the simplest damn mesh possible.
  int nx = 3, ny = 3, nz = 3;
  bbox_t bbox = {.x1 = 0.0, .x2 = 1.0,  // <-- physical bounding coordinates
                 .y1 = 0.0, .y2 = 1.0,
                 .z1 = 0.0, .z2 = 1.0};
  mesh_t* simple = create_uniform_mesh(MPI_COMM_WORLD, nx, ny, nz, &bbox);

  // Now create a SILO file for visualization, and write the mesh to it.
  silo_file_t* silo = silo_file_new(MPI_COMM_WORLD, "meshy", ".", 1, 0, 0, 0.0);
  silo_file_write_mesh(silo, "simple", simple);
  silo_file_close(silo);

  // Clean up.
  mesh_free(simple);

  return 0;
}
