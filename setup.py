from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import subprocess
import sys


class CMakeBuild(build_ext):

    def run(self):
        # make sure cmake is run first
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
            
        source_dir = os.path.abspath(os.path.dirname(__file__))

        cmake_args = [
            '-DCMAKE_BUILD_TYPE=Release',
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DMKVG_DO_PYTHON_BINDINGS=ON',
        ]

        build_args = ['--config', 'Release', '--', '-j8']
        subprocess.check_call(['cmake', source_dir] + cmake_args,
                              cwd=self.build_temp)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)


setup(
    name='monkvg_py',
    version='0.0.1',
    author='Micah Pearlman',
    author_email='micahpearlman@gmail.com',
    description='Python bindings for monkvg',
    ext_modules=[Extension('monkvg_py', ['bindings/python/monkvg_py.cpp'])],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
