from telemetry_plot import TelemetryPlot
from util import pos_inside_rect
from protobuf_readouts import ReadoutItem

ROWS = 2
COLUMNS = 3
MAX_PLOTS = ROWS * COLUMNS

class TelemetryPlots:
    def __init__(self, app):
        self.app = app
        #  self.l_power = TelemetryPlot('left pow',row=0,col=0,tick_increment=3)
        #  self.r_power = TelemetryPlot('right pow',row=0,col=1,tick_increment=3)
        #  self.robot_angle = TelemetryPlot('robot angle',row=1,col=0,tick_increment=40)
        #  self.angle_error = TelemetryPlot('angle error',row=1,col=1,tick_increment=40)
        #  self.plots = [self.l_power, self.r_power, self.robot_angle, self.angle_error]
        #  self.plots = [self.l_power, self.r_power]
        self.plots = []

    def render_init(self, screen):
        for plot in self.plots:
            plot.render_init(screen)

    def append_plot_if_fits(self, new_proto: ReadoutItem):
        if len(self.plots) < MAX_PLOTS:
            row = int(len(self.plots)/COLUMNS)
            col = len(self.plots) - (row) * COLUMNS
            self.plots.append(TelemetryPlot(new_proto, row, col, 3))
            self.plots[-1].render_init(self.app.screen)
            return True
        return False
    
    def replace_plot(self, i, new_proto):
        if new_proto is None:
            for j in range(i,len(self.plots)):
                print(j)
                if j+1 < len(self.plots):
                    self.plots[j] = TelemetryPlot(self.plots[j+1].pb_item,
                                                  self.plots[j].row,
                                                  self.plots[j].col, 3)
                    self.plots[j].render_init(self.app.screen)
                else:
                    self.plots[j].erase(self.app.screen)
                    del self.plots[j]
            return True
        self.plots[i] = TelemetryPlot(new_proto,
                                      self.plots[i].row,
                                      self.plots[i].col, 3)
        self.plots[i].render_init(self.app.screen)
        self.app.previously_clicked_item = None

    def render(self, screen):
        for plot in self.plots:
            plot.erase_update_render(screen)

    def handle_click(self, pos):
        for i,plot in enumerate(self.plots):
            if pos_inside_rect(pos, plot.rect):
                self.replace_plot(i, self.app.previously_clicked_item)
                return True
