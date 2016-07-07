import bpy
from mathutils import Matrix

# It creates a LED grid with a camera and a sample to be illuminated.
def create_led_grid(sc):
    N_H_LEDS = 8
    N_V_LEDS = 8
    DIST = 10  # Distance between LEDs.
    X_CENTER = 0
    Y_CENTER = 0
    Z_CENTER = 40
    RADIUS_LED_CYL = 4
    DEPTH_LED_CYL = 0
    SAMPLE_HEIGHT = 10
    SCALE_SAMPLE = 2
    led_grid_coord = list()  # List of tuples with all coordinates of the
                             # the LEDs.
    # Determination of limit values for the grid coordinates.
    h_limit_x = (N_H_LEDS * 10) / 2
    l_limit_x = -h_limit_x
    h_limit_y = (N_H_LEDS * 10) / 2
    l_limit_y = -h_limit_y
    h_limit_x_offset = h_limit_x + X_CENTER
    l_limit_x_offset = l_limit_x + X_CENTER
    h_limit_y_offset = h_limit_y + Y_CENTER
    l_limit_y_offset = l_limit_y + Y_CENTER
    # (x, y) as below represents the coordinate for the first LED in x-plane
    # and z-plane.
    x = l_limit_x_offset
    y = l_limit_y_offset
    # Fill the led_grid_coor ('list').
    for i in range(N_H_LEDS + 1):
        for j in range(N_V_LEDS):
            led_grid_coord.append((x, y))
            x += DIST
        x = l_limit_x_offset
        y += DIST
    for i in range(len(led_grid_coord)):
        x, y = led_grid_coord[i]
        lamp_name = "lamp" + str(i + 1)
        lamp_data = bpy.data.lamps.new(name=lamp_name, type='SPOT')
        lamp_object = bpy.data.objects.new(name=lamp_name, object_data=lamp_data)
        sc.objects.link(lamp_object)
        lamp_object.location = (x, y, Z_CENTER)
        lamp_object.select = True
        sc.objects.active = lamp_object
        # Base model for LEDsdb.
        bpy.ops.mesh.primitive_cylinder_add(location=(x, y, Z_CENTER), radius=RADIUS_LED_CYL, depth=DEPTH_LED_CYL)
    # Create the camera.
    cam_name = "camgrid"
    cam_data = bpy.data.cameras.new(name=cam_name)
    cam_object = bpy.data.objects.new(name=cam_name, object_data=cam_data)
    scene.objects.link(cam_object)
    cam_object.location = (X_CENTER, Y_CENTER, Z_CENTER - 1)
    sc.objects.active = cam_object
    # The sample.
    bpy.ops.mesh.primitive_cylinder_add(location=(X_CENTER, Y_CENTER, 0), radius=RADIUS_LED_CYL * SCALE_SAMPLE, depth=SAMPLE_HEIGHT, rotation=(0, 1, 1))

if __name__ == '__main__':
    scene = bpy.context.scene
    create_led_grid(scene)
