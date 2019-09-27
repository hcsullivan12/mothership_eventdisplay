'''
Encoder emulator for testing

hunter
'''

class Emulator:
    def __init__(self):
        """
        We are only using x1 and x3
        """
        self.ball_inc = 0.2 * 0.508 # cm / rev
        self.steps = 0
        self.data = (0,0,0,0) # ang, NA, lin, NA 

    def getData(self):
        return self.data
    
    def setData(self, var, steps = 0):
        if var == 'a':
            """clockwise rotation"""
            self.data = [sum(x) for x in zip(self.data, [steps, 0, 0, 0])]
        elif var == 'b':
            """counterclockwise rotation"""
            self.data = [sum(x) for x in zip(self.data, [-1 * steps, 0, 0, 0])]
        elif var == 'e':
            """backward linear"""
            self.data = [sum(x) for x in zip(self.data, [0, 0, -1 * steps, 0])]
        elif var == 'f':
            """forward linear"""
            self.data = [sum(x) for x in zip(self.data, [0, 0, steps, 0])]
        else: 
            return