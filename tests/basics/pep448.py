print([1, 2, 3])
print([1, *[2, 3]])
print([*[1, 2], 3])
print([*[1], *[2], *[3]])
print([*[1], *[], *[2], *[3]])
print([*range(1, 4)])
print([1, *range(2, 4)])
print([*range(1, 3), 3])
print([1, *range(2, 3), 3])

print([1, *(2, 3)])
# print((1, *[2, 3]))  # FIXME

def foo(*p, **kw):
    print((p, kw))

foo(1)
foo(1, 2, 3)
foo(*[1, 2, 3])
foo(1, *[2, 3])
foo(*[1, 2], 3)
foo(*[1], *[2], *[3])
# foo(*[1], *[], *[2], *[3])  # FIXME
foo(*range(1, 4))