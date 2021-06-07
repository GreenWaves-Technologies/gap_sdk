import re
import hashlib

def get_reasonable_name(name, name_cache=None, anonymise=False, length=30):
    uniq = hashlib.sha1(name.encode("UTF-8")).hexdigest()[:8]
    if anonymise:
        valid_name = f"node_{uniq}"
    else:
        valid_name = f"{re.sub(r'[^a-zA-Z0-9_]', '', name)[0:length:]}"
        if name_cache is not None and valid_name not in name_cache:
            name_cache.add(valid_name)
            return valid_name
        valid_name += f'_{uniq}'
    if name_cache is None:
        return valid_name
    if valid_name not in name_cache:
        name_cache.add(valid_name)
        return valid_name
    idx = 0
    while f'{valid_name}_{idx}' in name_cache:
        idx += 1
    
    valid_name = f'{valid_name}_{idx}'
    name_cache.add(valid_name)
    return valid_name
