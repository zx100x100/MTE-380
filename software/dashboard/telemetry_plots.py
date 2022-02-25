from telemetry_plot import TelemetryPlot

class TelemetryPlots:
    def __init__(self):
        self.l_power = TelemetryPlot('left pow',row=0,col=0,tick_increment=3)
        self.r_power = TelemetryPlot('right pow',row=0,col=1,tick_increment=3)
        self.robot_angle = TelemetryPlot('robot angle',row=1,col=0,tick_increment=40)
        self.angle_error = TelemetryPlot('angle error',row=1,col=1,tick_increment=40)
        self.plots = [self.l_power, self.r_power, self.robot_angle, self.angle_error]

    def render_init(self, screen):
        for plot in self.plots:
            plot.render_init(screen)

    def render(self, screen):
        for plot in self.plots:
            plot.erase_update_render(screen)
