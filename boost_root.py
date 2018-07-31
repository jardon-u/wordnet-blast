
import re
import subprocess

package_folder_re = re.compile(".*Package folder:\s+(.*)")


if __name__ == '__main__':
    p = subprocess.Popen(["conan", "install", "boost/1.66.0@conan/stable"], stdout=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode == 0:
        package_folder = stdout.splitlines()[-1]
        m = package_folder_re.match(package_folder)
        print(m.group(1))
