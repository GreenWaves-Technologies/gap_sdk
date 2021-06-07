# import sys
# import pkgutil
# import importlib
# import os


# def import_submodules(package_name, package_root=None, recursive=True, package_path=None):
#     if package_path is None:
#         package_path = []
#         package_root = package_name
#         package = importlib.import_module(package_name)
#     else:
#         package = importlib.import_module(package_name,
#                                           package=package_root)

#     results = {}
#     for loader, name, is_pkg in pkgutil.walk_packages(package.__path__):
#         relative_name = '.'.join(package_path) + '.' + name
#         results[relative_name] = importlib.import_module(
#             relative_name,
#             package=package_root)
#         if recursive and is_pkg:
#             results.update(import_submodules(
#                 relative_name,
#                 package_root=package_root,
#                 recursive=recursive,
#                 package_path=package_path + [name]))
#     return results

# def import_submodules(package_name):
#     """ Import all submodules of a module, recursively

#     :param package_name: Package name
#     :type package_name: str
#     :rtype: dict[types.ModuleType]
#     """
#     package = sys.modules[package_name]
#     return {
#         name: importlib.import_module(package_name + '.' + name)
#         for loader, name, is_pkg in pkgutil.walk_packages(package.__path__)
#     }

# def list_submodules(dirpath, current_mod_path):
#     mods = []
#     for loader, name, is_pkg in pkgutil.walk_packages(dirpath):
#         modpath = current_mod_path + [name]
#         if is_pkg:
#             mods.extend(list_submodules(
#                 [os.path.join(loader.path, name)], modpath))
#         else:
#             mod = '.'.join(modpath)
#             importlib.import_module(mod)
#             mods.append(mod)
#     return mods


# t = import_submodules(__name__)

# __all__ = [_mod[1] for _mod in pkgutil.iter_modules(__path__) if not _mod[2]]

from .bindings import *
from .kernels import *
from .globals import *
