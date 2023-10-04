# 3rd Party
import PySimpleGUI as sg

sg.change_look_and_feel('Dark Blue 3')

# BotBot
from utils import Point, BotState, BotParams, transform_vector, ObjectType, G, calc_distance


class ArenaGUI:
    def __init__(self):
        self._markers = []
        pad = 0.1
        bot_left = Point(-pad, -pad)
        top_right = Point(G.ARENA_W + pad, G.ARENA_H + pad)

        # Edit these depending on your monitor resolution
        self._canvas_width = top_right.x * 1000
        self._canvas_height = top_right.y * 1000

        layout = [[sg.Graph(canvas_size=(self._canvas_width, self._canvas_height),
                            graph_bottom_left=bot_left,
                            graph_top_right=top_right,
                            background_color='grey', key='graph', float_values=True, enable_events=True,
                            drag_submits=True)]]

        self._window = sg.Window('Simulation Track', layout, grab_anywhere=True)
        self._canvas = self._window['graph']
        self._window.Finalize()

        self._cones = []
        # self._draw_cones()

        self._car_poly = [(- BotParams.length / 2, - BotParams.track_width / 2),
                          (- BotParams.length / 2, + BotParams.track_width / 2),
                          (+ BotParams.length / 2, + BotParams.track_width / 2),
                          (+ BotParams.length, 0),
                          (+ BotParams.length / 2, - BotParams.track_width / 2)]
        self._vehicle_fig = None
        self._vehicle_state = BotState()
        self._sensor_figs = []
        self._walls = []
        self._walls.append(self._canvas.DrawLine((0.0, 0.0), (0.0, G.ARENA_H), color='black', width=5))
        self._walls.append(self._canvas.DrawLine((0.0, G.ARENA_H), (G.ARENA_W, G.ARENA_H), color='black', width=5))
        self._walls.append(self._canvas.DrawLine((G.ARENA_W, G.ARENA_H), (G.ARENA_W, 0.0), color='black', width=5))
        self._walls.append(self._canvas.DrawLine((G.ARENA_W, 0.0), (0.0, 0.0), color='black', width=5))

        self._walls.append(self._canvas.DrawLine(G.OBSTACLE[0], G.OBSTACLE[1], color='black', width=5))
        # self._walls.append(self._canvas.DrawLine((0.0, G.ARENA_H), (G.ARENA_W, G.ARENA_H), color='black', width=5))
        # self._walls.append(self._canvas.DrawLine((G.ARENA_W, G.ARENA_H), (G.ARENA_W, 0.0), color='black', width=5))
        # self._walls.append(self._canvas.DrawLine((G.ARENA_W, 0.0), (0.0, 0.0), color='black', width=5))

    def event_loop(self):
        event, _ = self._window.read(0)
        if event is None or event == 'Exit':
            self._window.close()
            exit()

    def update_vehicle(self, car_state):
        self._canvas.delete_figure(self._vehicle_fig)
        rotated_car_poly = []
        for pt in self._car_poly:
            rotated_car_poly.append(transform_vector(pt, car_state.x_pos, car_state.y_pos, car_state.yaw))
        self._vehicle_fig = self._canvas.DrawPolygon(rotated_car_poly, fill_color='pink')

    def draw_map(self, envmap):
        for obj in envmap:
            self._markers.append(self._canvas.DrawPoint((obj.x, obj.y), size=obj.diameter,
                                                        color='blue' if obj.typ is ObjectType.dummy else 'red'))

    def update_sensors(self, p1, p2):
        for sensor in self._sensor_figs:
            self._canvas.delete_figure(sensor)

        for i in range(len(p1)):
            dis = calc_distance(p1[i][0], p1[i][1], p2[i][0], p2[i][1])
            if dis < 0.1:
                self._sensor_figs.append(self._canvas.DrawLine(p1[i], p2[i], color='red', width=5))
            elif dis < 0.2:
                self._sensor_figs.append(self._canvas.DrawLine(p1[i], p2[i], color='orange', width=5))
            else:
                self._sensor_figs.append(self._canvas.DrawLine(p1[i], p2[i], color='yellow', width=5))
