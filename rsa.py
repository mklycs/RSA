import random

def fermat_test(n, k):
    if n == 2:
        return True
    if (n % 2) == 0:
        return False
    for _ in range(k):
        a = random.randint(1, n-1)
        if pow(a, n-1, n)!= 1:
            return False
    return True

def get_prime(lower, upper):
    while True:
        p = random.randint(lower, upper)
        if fermat_test(p, 10):
            return p

def keys(min, max):
    p = get_prime(min, max)
    q = get_prime(min, max)

    while q == p:
        q = get_prime(min, max)

    N = (p * q)
    phi = (p - 1) * (q - 1)

    e = 65537
    d = pow(e, -1, phi)

    public_key = (e, N)
    private_key = (d, N)

    return public_key, private_key

def encrypt(m, public_key):
    e, N = public_key
    c = [(ord(char) ** e) % N for char in m]
    return c

def decrypt(c, private_key):
    d, N = private_key
    m = [chr((int ** d) % N) for int in c]
    return ''.join(m)

def main():
    msg = "get in IT"
    public_key, private_key = keys(100, 5000)
    print(public_key, private_key)
    c = encrypt(msg, public_key)
    m = decrypt(c, private_key)
    print(c)
    print(m)

if __name__ == "__main__":
    main()