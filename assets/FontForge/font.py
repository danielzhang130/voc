import fontforge
F = fontforge.open("RobotoMono-Regular.ttf")
n = ["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]
for name in n:
    filename = name + ".png"
    F[name].export(filename, 52)
