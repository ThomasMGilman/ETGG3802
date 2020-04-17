from colliders import *
from vector import *
import pygame


pygame.init()
win_w = 1000
win_h = 600
win = pygame.display.set_mode((win_w, win_h))
done = False
objects = []
objects.append(BoxCollider(Vector2(100, 50), Vector2(400, 300), (255, 0, 0)))
objects.append(BoxCollider(Vector2(30, 30), Vector2(100, 100), (0, 255, 0)))
objects.append(SphereCollider(50, Vector2(600, 100), (50, 50, 255)))
objects.append(SphereCollider(60, Vector2(600, 500), (255, 255, 0)))
selected_object = None
font = pygame.font.SysFont("Courier New", 16)
clock = pygame.time.Clock()
rotation_rate = 90
move_speed = 100
mouse_offset = None
colliding = set()
ray_origin = None
ray_dist = None
ray_direction = None
ray_end = None
click_state = None

while not done:
    # UPDATES
    delta_time = clock.tick() / 1000.0
    # ...look for object/object collisions
    colliding = set()
    for i in range(len(objects)):
        for j in range(i + 1, len(objects)):
            if collide(objects[i], objects[j]):
                colliding.add(objects[i])
                colliding.add(objects[j])
    # ...look for object/ray collisions
    ray_hits = []
    if ray_origin != None and ray_direction != None:
        for o in objects:
            ray_hits += o.ray_collision(ray_origin, ray_direction, ray_dist)

    # INPUT
    evt = pygame.event.poll()
    if evt.type == pygame.QUIT:
        done = True
    elif evt.type == pygame.KEYDOWN:
        if evt.key == pygame.K_ESCAPE:
            done = True
    if evt.type == pygame.MOUSEBUTTONDOWN:
        mpos = Vector2(*evt.pos)

        if evt.button == 1:
            if click_state == "ray":
                click_state = None
            elif click_state == "shape":
                click_state = None
                selected_object = None
            elif selected_object == None:
                # See if we are clicking on a shape
                for i in range(len(objects)):
                    if objects[i].point_inside(mpos):
                        selected_object = i
                        mouse_offset = objects[selected_object].mPosition - mpos
                        click_state = "shape"
                        break

                if selected_object == None:
                    # If we get here, the user clicked, but not on an object.  Start a Ray here
                    ray_origin = mpos
                    click_state = "ray"

        elif evt.button in (4, 5) and selected_object != None:
            if evt.button == 4:
                objects[selected_object].rotate(15)
            else:
                objects[selected_object].rotate(-15)

    if click_state == "shape":
        mpos = Vector2(*pygame.mouse.get_pos())
        objects[selected_object].mPosition = mpos + mouse_offset
    elif click_state == "ray":
        ray_end = Vector2(*pygame.mouse.get_pos())
        offset = ray_end - ray_origin
        ray_dist = (ray_end - ray_origin).magnitude()
        if ray_dist > 0:
            ray_direction = offset.normalized()

    # DRAWING
    win.fill((0,0,0))
    for i in range(len(objects)):
        o = objects[i]
        o.draw(win, i == selected_object, objects[i] in colliding)
    if ray_origin != None and ray_direction != None:
        #pygame.draw.line(win, (255, 255, 255), ray_origin.i, (ray_origin + ray_direction * 2000).i)
        pygame.draw.line(win, (255, 255, 255), ray_origin.i, ray_end.i)
        pygame.draw.circle(win, (255, 255, 255), ray_origin.i, 5)
        for pt in ray_hits:
            pygame.draw.circle(win, (255, 255, 255), pt.i, 5, 1)

    win.blit(font.render("[LClick] to select / clear selection, [ScrWheel]: rotate selected", False, (128, 128, 128)), (0, win_h - 40))
    win.blit(font.render("[LClick] on an empty area to start a ray and then point it / stop aiming", False, (128, 128, 128)),
             (0, win_h - 20))



    pygame.display.flip()

pygame.quit()