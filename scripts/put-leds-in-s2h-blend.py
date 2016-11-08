__author__ = "Jose David Marroquin Toledo"
__credits__ = ["Jose David Marroquin Toledo", ]
__email__ = "jose@marroquin.cl"
__status__ = "Development"

import math
import bpy
import blendjupyter as blendj
import illuminator as led

if __name__ == '__main__':
    leds = [12, 18, 36]
    l = led.gen_xyz_led_ring([12, 18, 36], 5, 0)
    parent_name = 'S2H-LED-ring-illuminator'
    led_stl_path = '/home/marroquintoledo-l-br/Escritorio/LED_prop.stl'
    blendj.put_mesh(led_stl_path, l, copies=sum(leds), rotation=(180, 0, 0),
                    parent=parent_name, offset=(0, 0, -1.51346))