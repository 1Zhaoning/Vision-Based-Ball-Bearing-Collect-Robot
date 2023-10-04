from utils import Object, ObjectType, Point

class Scanner:
    def scan_arena(self,envmap):
        targets = []
        for obj in envmap:
            if obj.typ == ObjectType.target:
                iden_target = Point(obj.x, obj.y)
                targets.append(iden_target)                
        return targets
