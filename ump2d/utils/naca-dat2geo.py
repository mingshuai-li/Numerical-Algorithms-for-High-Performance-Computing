import os
import sys
import shutil
import numpy as np
from io import StringIO   
import argparse


class Axis:
  X = 0
  Y = 1


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('-i', '--input', type=str, help='naca dat file')
  parser.add_argument('-o', '--output', type=str, help='output geo file')
  args = parser.parse_args()

  if not os.path.exists(args.input):
    sys.exit('file does not exist')

  with open(args.input, 'r') as file:
    content = file.readlines()
  
  num_points = len(content[1:])
  points = np.zeros([2, num_points])

  for id, line in enumerate(content[1:]):
    point2d = list(filter(None, line[:-1].split(' ')))
    point2d = [float(value) for value in  point2d]
    points[Axis.X][id] = float(point2d[Axis.X])
    points[Axis.Y][id] = float(point2d[Axis.Y])

  output_stream = StringIO()
  output_stream.write(f'lc = 5e-3;\n')
  output_stream.write('nc = 1e-1;\n')

  for id in range(num_points):
    output_stream.write(f'Point({id}) = {{{points[0][id]}, {points[1][id]}, 0.0, lc}};\n')
  
  max_x = np.max(points[Axis.X][:])
  max_y = np.max(points[Axis.Y][:])
  min_x = np.min(points[Axis.X][:])
  min_y = np.min(points[Axis.Y][:])
  dx = max_x - min_x
  dy = max_y - min_y 
  
  factor_x = 3
  factor_y = 14
  domain = [[max_x - factor_x * dx, max_y + factor_y * dy],
            [max_x + factor_x * dx, max_y + factor_y * dy],
            [max_x + factor_x * dx, max_y - factor_y * dy],
            [max_x - factor_x * dx, max_y - factor_y * dy]]
  
  for i, point in enumerate(domain):
    id = i + num_points
    output_stream.write(f'Point({id}) = {{{point[Axis.X]}, {point[Axis.Y]}, 0.0, nc}};\n')


  for i in range(num_points):
    first_point = i
    second_point = (i + 1) % num_points
    output_stream.write(f'Line({i}) = {{{first_point}, {second_point}}};\n')
  
  for i in range(4):
    line_id = i + num_points
    first_point = line_id
    increment = (i + 1) % 4
    second_point = num_points + increment
    output_stream.write(f'Line({line_id}) = {{{first_point}, {second_point}}};\n')
  
  all_points_ids = [str(id) for id in range(num_points)]
  all_points_ids = ','.join(all_points_ids)
  output_stream.write(f'Curve Loop(100) = {{{all_points_ids}}};\n')

  domain_points_ids = [str(id + num_points) for id in range(4)]
  domain_points_ids = ','.join(domain_points_ids)
  output_stream.write(f'Curve Loop(101) = {{{domain_points_ids}}};\n')

  output_stream.write(f'Plane Surface(200) = {{100, 101}};\n')

  output_stream.write('Physical Surface("domain") = {200};')

  output_path = os.path.abspath(args.output)
  output_path = os.path.dirname(output_path)
  if not os.path.exists(output_path):
    os.makedirs(output_path, exist_ok=True)

  with open(f'{args.output}.geo', 'w') as file:
    file.write(output_stream.getvalue())

if __name__ == '__main__':
  main()
