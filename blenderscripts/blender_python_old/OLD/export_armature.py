
import Blender
import os.path



########
# main #
########
def main():

	print "\n---- Export Armature ----"

	########
	# vars #
	########
	file = 0
	path = "c:\\src\\VisualC++\\gmdl_3d_format\\models\\satan\\"
	if not os.path.exists( path ): path = "c:\\data\\projects\\VisualC++\\gmdl_3d_format\\models\\satan\\"
	armat = 0
	mesh = 0


	##################################
	# make some checks and open file #
	##################################
	objs = Blender.Object.GetSelected()

	if len( objs ) < 1: 
		print "-->ERROR: No object selected"
		return 0
	
	if objs[0].getType() != "Mesh": 
		print "-->ERROR: The selected object must be a mesh and not in a(n) " + objs[0].getType()
		return 0

	mesh = objs[0].getData(0,1)

	if objs[0].getParent() == "":
		print "-->ERROR: Mesh doesnt have a parent"
		return 0		

	if Blender.Object.Get( objs[0].getParent().name ).getType() != "Armature": 
		print "-->ERROR: The parent must be an armature and not in a(n) " + Blender.Object.Get( objs[0].getParent().name ).getType()
		return 0

	armat = Blender.Object.Get( objs[0].getParent().name ).getData( 0, 1 )
	
	filename = path + "armature.txt"
	file = open( filename, "w" )
	
	
	###############
	# write bones #
	###############	
	bone_names = armat.bones.keys()
	
	file.write( "BONES_NUM " + str(len( bone_names )) + "\n" )

	i = 0
	bone_names.sort() # the bones are written in alpabetical order
	for bone_nam in bone_names:
		
		vgroup_names = mesh.getVertGroupNames()
		vgroup_names.sort()
		vg_i = 0
		for vgroup_nam in vgroup_names:
			if vgroup_nam == bone_nam:
				break
			vg_i = vg_i+1
		if vg_i == len(vgroup_names): vg_i=-1
			
		
		bone = armat.bones[ bone_nam ]
		file.write( "BONE %d NAME %s VGROUP_INDEX %d " %(i, bone.name, vg_i) )
		
		co = bone.head["ARMATURESPACE"]
		file.write( "HEAD %f %f %f " %(co.x, co.y, co.z) )
		
		co = bone.tail["ARMATURESPACE"]
		file.write( "TAIL %f %f %f\n" %(co.x, co.y, co.z) )

		i = i+1
	

	print "Done! File \"%s\" created" % filename
	file.close()
main()