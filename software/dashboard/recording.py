import pygame as pg
import time

DIAM = 8
PAD = 2
TICKS_BETWEEN_BLINKS = 50

class Recording:
    def __init__(self, app):
        self.app = app
        self.rect = pg.Rect(0,0,DIAM+PAD,DIAM+PAD)
        self.rect.bottomright = self.app.screen.get_rect().bottomright
        self.on_image = self.generate_on_image()
        self.off_image = self.generate_off_image()
        self.ticks_since_blinked = 0
        self.last_enabled = False

    @property
    def enabled(self):
        return self.app.data.recording_to_dirname is not None
        
    def generate_on_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill((0,0,0,0))
        rect = image.get_rect()
        pg.draw.circle(image, (255,0,0), rect.center, int(DIAM/2))
        return image
    
    def generate_off_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill((0,0,0,0))
        rect = image.get_rect()
        pg.draw.circle(image, (0,0,0), rect.center, int(DIAM/2))
        return image
    
    def render(self):
        if self.enabled:
            if not self.last_enabled:
                self.app.screen.blit(self.on_image, self.rect)
                self.ticks_since_blinked = 0
            else:
                self.ticks_since_blinked += 1
                if self.ticks_since_blinked == TICKS_BETWEEN_BLINKS/2:
                    self.app.screen.blit(self.off_image, self.rect)
                elif self.ticks_since_blinked == TICKS_BETWEEN_BLINKS:
                    self.app.screen.blit(self.on_image, self.rect)
                    self.ticks_since_blinked = 0
        else:
            if self.last_enabled:
                self.app.screen.blit(self.off_image, self.rect)
                self.app.telemetry_client.regenerate_playback_filenames()
                time.sleep(0.3)
                self.app.telemetry_client.playback_dir_num += 1
                self.app.telemetry_client.regenerate_playback_filenames()

        self.last_enabled = self.enabled
