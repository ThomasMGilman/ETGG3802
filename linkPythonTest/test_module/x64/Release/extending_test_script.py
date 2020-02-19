import test_module

print(dir(test_module))         # ['__doc__', '__file__', '__loader__', '__name__', '__package__',
                                #      '__spec__', 'mystery_val', 'test']
print(test_module.__doc__)      # This is the docstring for my super-awesome module.
print(test_module.__name__)     # fake-name
print(test_module.test.__doc__) # My super-awesome function
print(test_module.mystery_val)  # 42
print(test_module.test("this is a really long string", "s"))    # (3, 6, 22)
print(test_module.test("this is a really long string", "is"))   # (2, 5)
print(test_module.test("this is a really long string", "xyz"))  # None

# Each of these, if uncommented, should raise the following exception:
#Traceback (most recent call last):
#  File "D:\wherever_it_is\test_script.py", line 14, in <module>
#    print(test_module.test("abc"))
#ValueError: NOOO -- that's not the right way to call this!
#print(test_module.test("abc"))
#print(test_module.test("abc", 4))
