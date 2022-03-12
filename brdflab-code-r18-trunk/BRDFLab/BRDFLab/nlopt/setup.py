from distutils.core import setup, Extension
nlopt_module = Extension('_nlopt',
                           sources=['nlopt-python.cpp'],
                           libraries=['libnlopt-0'],
                           )
setup (name = 'nlopt',
       version = '2.1.1',
       author      = "Steven G. Johnson",
       description = """NLopt nonlinear-optimization library""",
       ext_modules = [nlopt_module],
       py_modules = ["nlopt"],
       )
