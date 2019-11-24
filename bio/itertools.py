def first(it):
    i = iter(it)
    try:
        return next(i)
    except StopIteration:
        raise Exception(f'Expected at least one item, got {it}')


def head(it, n=5):
    for i, val in enumerate(it, 1):
        if i > n:
            break
        yield val


def one(it):
    i = iter(it)
    err_msg = f'Expected one item, got {it}'
    try:
        ret_val = next(i)
    except StopIteration:
        raise Exception(err_msg)
    try:
        next(i)
    except StopIteration:
        return ret_val
    raise Exception(err_msg)
