with open("render_test.json", "r") as f:
    data = f.read()

json_data = "{\"tiles\": [" + data.replace("\n", ",").strip(",") + "]}"
import json
data = json.loads(json_data)

import pygame
pygame.init()
screen = pygame.display.set_mode((400, 400))
while True:
    for n in data["tiles"]:
        pygame.draw.rect(screen, [(n[2] + 1) * 120] * 3, ((n[0] + 64) * 4, (n[1] + 64) * 4, 4, 4))

    pygame.display.flip()
