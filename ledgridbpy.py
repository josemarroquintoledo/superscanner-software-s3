import bpy
from mathutils import Matrix

# It creates a LED grid with a camera and a sample to be illuminated.
def create_led_grid(sc, **kwargs):
    RADIUS_LED_CYL = 4
    DEPTH_LED_CYL = 0
    SAMPLE_HEIGHT = 10
    SCALE_SAMPLE = 2
    h_leds = kwargs.pop('h_leds', 8)  # It is equivalent to the number of
                                      # columns of the LED grid.
    v_leds = kwargs.pop('v_leds', 8)  # ... to the number of the rows.
    dist = kwargs.pop('dist', 10)  # Distance between LEDs.
    # The LED grid will be centered in the (x, y, z).
    x = kwargs.pop('x', 0)  # X coordinate in the x-axis.
    y = kwargs.pop('y', 0)  # Y coordinate in the y-axis.
    z = kwargs.pop('z', 40) # Z coordinate in the z-axis.
    led_grid_coord = list()  # List of tuples with all coordinates of the
                             # the LEDs.
    # Determination of the limits of the grid.
    h_limit_x = (h_leds * dist) / 2
    c_limit_x = -h_limit_x
    h_limit_y = (h_leds * dist) / 2
    c_limit_y = -h_limit_y
    h_limit_x_offset = h_limit_x + x
    c_limit_x_offset = c_limit_x + x
    h_limit_y_offset = h_limit_y + y
    c_limit_y_offset = c_limit_y + y
    # (x_init, y_init) as below represents the coordinate for the first LED in
    # the x-plane and the y-plane.
    x_init = c_limit_x_offset
    y_init = c_limit_y_offset
    # Fill the led_grid_coord ('list').
    for i in range(h_leds):
        for j in range(v_leds):
            led_grid_coord.append((x_init, y_init))
            x_init += dist
        x_init = c_limit_x_offset
        y_init += dist
    for i in range(len(led_grid_coord)):
        x_i, y_i = led_grid_coord[i]
        lamp_name = "lamp" + str(i + 1)
        # The lamp as cone.
        lamp_data = bpy.data.lamps.new(name=lamp_name, type='SPOT')
        lamp_object = bpy.data.objects.new(name=lamp_name,
                                           object_data=lamp_data)
        sc.objects.link(lamp_object)
        lamp_object.location = (x_i, y_i, z)
        lamp_object.select = True
        sc.objects.active = lamp_object
        # Base model for the LED.
        bpy.ops.mesh.primitive_cylinder_add(location=(x_i, y_i, z),
                                            radius=RADIUS_LED_CYL,
                                            depth=DEPTH_LED_CYL)
    # Create the camera.
    cam_name = "camgrid"
    cam_data = bpy.data.cameras.new(name=cam_name)
    cam_object = bpy.data.objects.new(name=cam_name, object_data=cam_data)
    scene.objects.link(cam_object)
    cam_object.location = (x, y, z - 1)
    sc.objects.active = cam_object
    # The sample.
    bpy.ops.mesh.primitive_cylinder_add(location=(x, y, 0),
                                        radius=RADIUS_LED_CYL * SCALE_SAMPLE,
                                        depth=SAMPLE_HEIGHT,
                                        rotation=(0, 1, 1))

# It deletes all objects of the current blend file.
def delete_all_objects():
    l_objects = list(bpy.data.objects)
    for o in l_objects:  # Select all objects.
        o.select = True
    bpy.ops.object.delete()  # Delete the selected objects.
           
if __name__ == '__main__':
    delete_all_objects()
    scene = bpy.context.scene
    create_led_grid(scene)