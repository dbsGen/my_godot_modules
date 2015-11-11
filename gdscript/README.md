Some improvements of gdscript.

    func _ready():
      
    	# Easier connection
    	self.connect("renamed", on_renamed)
    	
    	var hello = "hello"
    	# lambda expressions
    	var lambda_func = func():
    		print(hello)
    	
    	# or 
    	func lambda_func2():
    		print(hello)
    		
    	# lambda_func is a function object
    	lambda_func.apply()
    	lambda_func2.apply()
    	
    	# A native function object
    	var native_func = self.set_name
    	native_func.apply("new_name")
    	
    	
    func on_renamed():
      print(get_name())
