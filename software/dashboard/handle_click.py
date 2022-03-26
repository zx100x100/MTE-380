import pygame as pg

def handle_click(app, mouse_presses):
    if not mouse_presses[0]: # LEFT MOUSE BUTTON CLICKED
        return
    pos = pg.mouse.get_pos()
    clicked_tile = app.arena.handle_click(pos)
    if clicked_tile:
        # update cmdData!!
        found = False
        for i,pos in enumerate(zip(app.data.cmd.pb.trapX, app.data.cmd.pb.trapY)):
            posx, posy = pos 
            if posx == clicked_tile[0]+0.5 and posy == clicked_tile[1]+0.5:
                app.data.cmd.pb.trapX[i] = -1
                app.data.cmd.pb.trapY[i] = -1
                app.data.cmd.pb.nTraps -= 1
                found = True
        if not found:
            replaced = False
            for i,pos in enumerate(zip(app.data.cmd.pb.trapX, app.data.cmd.pb.trapY)):
                posx, posy = pos 
                if posx == -1 and posy == -1:
                    replaced = True
                    app.data.cmd.pb.nTraps += 1
                    app.data.cmd.pb.trapX[i] = clicked_tile[0]+0.5
                    app.data.cmd.pb.trapY[i] = clicked_tile[1]+0.5
                    break
            if not replaced:
                app.arena.tiles[posy]
        return
    if app.controls.handle_click(pos):
        return
    if app.telemetry_plots.handle_click(pos):
        if app.previously_clicked_item:
            app.previously_clicked_item.set_not_clicked()
        return
    if app.playback.handle_click(pos):
        return
    item = app.protobuf_readouts.handle_click(pos)
    if item:
        if not item.is_numeric:
            return
        if not app.telemetry_plots.append_plot_if_fits(item):
            if app.previously_clicked_item:
                app.previously_clicked_item.set_not_clicked()
            app.previously_clicked_item = item
            item.set_clicked()
        else:
            app.previously_clicked_item = None
