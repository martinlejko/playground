def is_palidrom(word):
    if len(word) <= 1:
        return True
    elif word[0] == word[-1]:
        return is_palidrom(word[1:-1])
    else:
        return False


print(is_palidrom("MARTIN"))
print(is_palidrom("ADA"))
