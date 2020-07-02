# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import numpy as np

# indices
CNTY_IDX, CNTX_IDX, H_IDX, W_IDX = 0, 1, 2, 3
YMIN_IDX, XMIN_IDX, YMAX_IDX, XMAX_IDX = 0, 1, 2, 3

def convert_cors2cnts(bboxes_cors):
    bboxes_cnts = np.zeros_like(bboxes_cors)
    bboxes_cors[:, H_IDX] = bboxes_cnts[:, YMAX_IDX] - bboxes_cnts[:, YMIN_IDX]
    bboxes_cors[:, W_IDX] = bboxes_cnts[:, XMAX_IDX] - bboxes_cnts[:, XMIN_IDX]
    bboxes_cors[:, CNTY_IDX] = bboxes_cnts[:, YMIN_IDX] + 0.5 * bboxes_cors[:, H_IDX]
    bboxes_cors[:, CNTX_IDX] = bboxes_cnts[:, XMIN_IDX] + 0.5 * bboxes_cors[:, W_IDX]
    return bboxes_cors

def rect_intersect_area(box_cors_a, box_cors_b):
    x = max(box_cors_a[XMIN_IDX], box_cors_b[XMIN_IDX])
    y = max(box_cors_a[YMIN_IDX], box_cors_b[YMIN_IDX])

    size_x = min(box_cors_a[XMAX_IDX], box_cors_b[XMAX_IDX]) - x
    size_y = min(box_cors_a[YMAX_IDX], box_cors_b[YMAX_IDX]) - y

    if size_x <= 0 or size_x <= 0:
        return 0
    else:
        if size_x.dtype in (np.float32, np.float64):
            return size_x * size_y
        else:
            return size_x.astype(np.int32) * size_y

def rect_union_area(box_cors_a, box_cors_b):
    x_a = abs(box_cors_a[XMAX_IDX] - box_cors_a[XMIN_IDX])
    y_a = abs(box_cors_a[YMAX_IDX] - box_cors_a[YMIN_IDX])
    x_b = abs(box_cors_b[XMAX_IDX] - box_cors_b[XMIN_IDX])
    y_b = abs(box_cors_b[YMAX_IDX] - box_cors_b[YMIN_IDX])

    if x_a.dtype in (np.float32, np.float64):
        area_a = x_a * y_a
        area_b = x_b * y_b
    else:
        area_a = x_a.astype(np.int32) * y_a
        area_b = x_b.astype(np.int32) * y_b
    return area_a + area_b - rect_intersect_area(box_cors_a, box_cors_b)
