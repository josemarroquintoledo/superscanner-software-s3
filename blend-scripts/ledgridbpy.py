import bpy
from mathutils import Matrix
import math
import os

# Set up some parameters of a scene, sc ('bpy.context.scene').
def set_up_sc(sc, **kwargs):
    wpx = kwargs.pop('wpx', 64)
    hpx = kwargs.pop('hpx', 64)
    antialias = kwargs.pop('antialias', '8')
    bw = kwargs.pop('bw', True)
    extension = kwargs.pop('extension', 'TIFF')
    sc.render.resolution_x = wpx
    sc.render.resolution_y = hpx
    sc.render.antialiasing_samples = antialias
    sc.render.use_overwrite = True
    sc.render.image_settings.file_format = extension
    if bw:
        sc.render.image_settings.color_mode = 'BW'
    else:
        sc.render.image_settings.color_mode = 'RGB'

# Delete all objects of the current blend file.
def delete_all_objects():
    l_objects = list(bpy.data.objects)
    for o in l_objects:  # Select all objects.
        o.select = True
    bpy.ops.object.delete()  # Delete the selected objects.

# "Return a string that contains a sequence n-zeros followed by num ('int') as
# 'str', for example, num_str_zeros(89, 4) returns '0089'". From
# fwdimaging.ipynb Jupyter notebook.
def num_str_zeros(num, n_digs, matlab=False):
    if matlab:  # Begin the numeration with 1 in the filename.
        num += 1
    len_num = len(str(num))
    str_num = ''
    for i in range(n_digs - len_num):
        str_num += '0'
    str_num += str(num)
    return str_num

# Return the number of LEDs per row and column, the camera ('bpy.types.Camera')
# and the list with (x, y, z) coordinates 'tuple') of the all LED of the grid.
def create_led_grid(sc, **kwargs):
    rows = kwargs.pop('rows', 15)  # Number of LEDs per row and column.
    dist = kwargs.pop('dist', 6)  # Distance in mm between LEDs.
    # The LED grid will be centered in the (x, y, z).
    xgrid = kwargs.pop('xgrid', 0)  # X coordinate of the grid in the x-axis.
    ygrid = kwargs.pop('ygrid', 0)  # ... Y in the y-axis.
    zgrid = kwargs.pop('zgrid', 40) # ... Z in the z-axis.
    radiusled = kwargs.pop('radiusled', 2.5)  # Radius in mm of the LED.
    led_grid_coord = list()  # List of tuples with all coordinates of the LEDs
                             # of the grid.
    # Determination of the limits of the grid when it is in the center.
    plimit_x = (rows * dist) / 2  # Positive horizontal limit for the X
                                  # coordinate.
    nlimit_x = -plimit_x  # Negative horizontal limit for the X coordinate.
    plimit_y = (rows * dist) / 2
    nlimit_y = -plimit_y
    # Determination of the limits of the grid when it is moved in x-axis and
    # y-axis.
    plimit_x_moved = plimit_x + xgrid
    nlimit_x_moved = nlimit_x + xgrid
    plimit_y_moved = plimit_y + ygrid
    nlimit_y_moved = nlimit_y + ygrid
    x_init = nlimit_x_moved  # X coordinate of the first LED of the grid.
    y_init = plimit_y_moved  # Y coordinate...
    # Fill the led_grid_coord ('list') with the coordinates of the LEDs.
    for i in range(rows):
        for j in range(rows):
            led_grid_coord.append((x_init, y_init, zgrid))
            x_init += dist
        x_init = nlimit_x_moved
        y_init -= dist
    for i in range(len(led_grid_coord)):
        x_i, y_i, z_i = led_grid_coord[i]
        lamp_name = "lamp" + num_str_zeros(i, len(str(rows ** 2)), matlab=True)
        # The lamp as a cone.
        lamp_data = bpy.data.lamps.new(name=lamp_name, type='SPOT')
        lamp_data.energy = 0.0 # The lamp if off.
        lamp_object = bpy.data.objects.new(name=lamp_name,
                                           object_data=lamp_data)
        sc.objects.link(lamp_object)
        lamp_object.location = (x_i, y_i, z_i)
        lamp_object.select = True
        sc.objects.active = lamp_object
        # Base model for the LED.
        bpy.ops.mesh.primitive_cylinder_add(location=(x_i, y_i, z_i),
                                            radius=radiusled,
                                            depth=0)
    # Create the camera.
    cam_name = "camgrid"
    cam_data = bpy.data.cameras.new(name=cam_name)
    cam_object = bpy.data.objects.new(name=cam_name, object_data=cam_data)
    scene.objects.link(cam_object)
    cam_object.location = (xgrid, ygrid, zgrid - 1)
    sc.objects.active = cam_object
    # cam_object ('bpy.types.Camera') MUST BE RETURNED to render the scene.
    return rows, cam_object, led_grid_coord

# Render a scene ('bpy.context.scene'), sc, with a camera ('bpy.types.Camera'),
# cam, and save the result in render_path ('str') as PNG file.
def take_photo(cam, sc, filename, outpath='img_lores/'):
    sc.camera = cam
    sc.render.filepath = outpath + filename
    bpy.ops.render.render(write_still=True)

# Turn on a lamp at time, take a photo of the mesh and save it.
def illuminate_step_by_step(rows, cam, sc, **kwargs):
    prefix = kwargs.pop('prefix', 'lores_')
    l_lamps = get_lamps()
    for i in range(len(l_lamps)):
        l_lamps[i].data.energy = 2.0
        take_photo(cam, sc, prefix + num_str_zeros(i, len(str(rows ** 2)),
                                                      matlab=True))
        l_lamps[i].data.energy = 0.0

# Return a list ('list') with all lamps ('bpy.types.Lamp') of the blend file.
def get_lamps(prefix='lamp'):
    l_lamps = list()
    for o in bpy.data.objects:
        if prefix in o.name:
            l_lamps.append(o)
    return l_lamps

if __name__ == '__main__':
    # Uncomment the line below and EXECUTE ONCE before import your mesh.
    # delete_all_objects()
    # UNCOMMENT ALL BELOW to render the scene.
    ''' scene = bpy.context.scene
    set_up_sc(scene)
    rows, cam, _ = create_led_grid(scene)
    illuminate_step_by_step(rows, cam, scene)
    '''