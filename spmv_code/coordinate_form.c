#include <stdio.h>
#include <stdlib.h>

#include "include/coordinate_form.h"

CoordinateForm *make_coordinate_form(const int num_non_zero)
{
  CoordinateForm *coordinate_form;

  coordinate_form = malloc(sizeof *coordinate_form);
  coordinate_form->length = num_non_zero;
  coordinate_form->values = malloc(num_non_zero * sizeof *coordinate_form->values);
  coordinate_form->row_indices = malloc(num_non_zero * sizeof *coordinate_form->row_indices);
  coordinate_form->col_indices = malloc(num_non_zero * sizeof *coordinate_form->col_indices);

  return coordinate_form;
}

void free_coordinate_form(CoordinateForm *coordinate_form)
{
  free(coordinate_form->values);
  free(coordinate_form->row_indices);
  free(coordinate_form->col_indices);
  free(coordinate_form);
}

CoordinateForm *get_coordinate_form(double **matrix, const Dimension matrix_dim,
                                    const int num_non_zero)
{
  CoordinateForm *coordinate_form = make_coordinate_form(num_non_zero);

  int index_non_zero = 0;
  for (int i = 0; i < matrix_dim.num_rows; i++)
  {
    for (int j = 0; j < matrix_dim.num_columns; j++)
    {
      double value = matrix[i][j];
      if (value != 0)
      {
        coordinate_form->values[index_non_zero] = value;
        coordinate_form->row_indices[index_non_zero] = i;
        coordinate_form->col_indices[index_non_zero] = j;
        index_non_zero++;
      }
    }
  }

  return coordinate_form;
}

void multiply_coordinate_form_with_vector(CoordinateForm *coordinate_form,
                                          double *vector, double *result)
{
  for (int i = 0; i < coordinate_form->length; i++)
  {
    result[coordinate_form->row_indices[i]] +=
        coordinate_form->values[i] * vector[coordinate_form->col_indices[i]];
  }
}

void print_coordinate_form(const CoordinateForm *coordinate_form)
{
  printf("| Values\t| Row indices\t| Col indices\t|\n");
  for (int i = 0; i < coordinate_form->length; i++)
  {
    printf("| %lf\t| %d\t\t| %d\t\t|\n", coordinate_form->values[i],
           coordinate_form->row_indices[i] + 1,
           coordinate_form->col_indices[i] + 1);
  }
  printf("\n");
}
