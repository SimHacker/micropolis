import os
os.environ["ARCADE_HEADLESS"] = "True"
import arcade
import micropolis_rs

# --- Constants ---
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
SCREEN_TITLE = "Micropolis"

WORLD_WIDTH = 120
WORLD_HEIGHT = 100
TILE_SIZE = 8

COLOR_MAP = {
    micropolis_rs.DIRT: arcade.color.BROWN,
    micropolis_rs.RIVER: arcade.color.BLUE,
    micropolis_rs.TREEBASE: arcade.color.DARK_GREEN,
    micropolis_rs.WOODS: arcade.color.DARK_GREEN,
    micropolis_rs.ROADBASE: arcade.color.GRAY,
    micropolis_rs.RESBASE: arcade.color.GREEN,
    micropolis_rs.COMBASE: arcade.color.LIGHT_BLUE,
    micropolis_rs.INDBASE: arcade.color.YELLOW,
}

class GameWindow(arcade.Window):
    """
    Main application window.
    """

    def __init__(self, width, height, title):
        super().__init__(width, height, title)
        arcade.set_background_color(arcade.color.BLACK)
        self.micropolis = micropolis_rs.Micropolis()
        self.frame_count = 0

    def setup(self):
        """ Set up the game and initialize the variables. """
        pass

    def on_draw(self):
        """
        Render the screen.
        """
        self.clear()

        map_view = self.micropolis.get_map_view()

        for y in range(WORLD_HEIGHT):
            for x in range(WORLD_WIDTH):
                tile_type = map_view[y * WORLD_WIDTH + x]
                color = COLOR_MAP.get(tile_type, arcade.color.BLACK)

                # For zone tiles, we need to check the base type
                if micropolis_rs.RESBASE <= tile_type < micropolis_rs.COMBASE:
                    color = COLOR_MAP.get(micropolis_rs.RESBASE, arcade.color.BLACK)
                elif micropolis_rs.COMBASE <= tile_type < micropolis_rs.INDBASE:
                    color = COLOR_MAP.get(micropolis_rs.COMBASE, arcade.color.BLACK)
                elif micropolis_rs.INDBASE <= tile_type < micropolis_rs.PORTBASE:
                    color = COLOR_MAP.get(micropolis_rs.INDBASE, arcade.color.BLACK)


                arcade.draw_rectangle_filled(
                    x * TILE_SIZE + TILE_SIZE / 2,
                    y * TILE_SIZE + TILE_SIZE / 2,
                    TILE_SIZE,
                    TILE_SIZE,
                    color,
                )

    def on_update(self, delta_time):
        """ Movement and game logic """
        self.micropolis.step_simulation()
        self.frame_count += 1
        if self.frame_count > 5:
            print("Taking screenshot...")
            image = arcade.get_image()
            image.save("screenshot.png", "PNG")
            print("Screenshot saved to screenshot.png")
            self.close()


def main():
    """ Main method """
    window = GameWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE)
    window.setup()
    arcade.run()


if __name__ == "__main__":
    main()
