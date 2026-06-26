import sys
import math

# ==========================================
# 1. Structure Definitions & Error Handling
# ==========================================

# struct point -> Represented as a standard Python list of floats.
# struct point_node -> Handled automatically by Python lists.

def error():
    print("An Error Has Occurred")
    sys.exit(1)

# print_output forward declaration in C -> Python defines it fully later down.

def creat_point(lst, d):
    """Creates a point (returns a deep copy list of length d)."""
    # Dynamic memory allocation check is handled natively by Python
    if len(lst) < d:
        error()
    return [float(x) for x in lst[:d]]


def copy_point(dest, src):
    """Copies the contents of src to dest."""
    # Assumes equal dimensions and that dest already exists
    for i in range(len(src)):
        dest[i] = src[i]

def distance(pt1, pt2):
    """Calculates Euclidean distance between two points."""
    d = len(pt1)
    if len(pt2) != d:
        # אף פעם לא אמור לקרות
        print("points arent the same dim")
        error()
        
    s = 0.0
    for i in range(d):
        diff = pt1[i] - pt2[i]
        s += diff * diff
        
    return math.sqrt(s)

def check_point_lst(points, d, N):
    """Validates the structure of the incoming data list."""
    if points is None or d <= 0 or N <= 0:
        error()
    for i in range(N):
        if len(points[i]) != d or points[i] is None:
            error()

# ==========================================
# 2. Input Parsing Functions
# ==========================================

"""
הרעיון של עיבוד הקלט פשוט יחסית:
להגדיר רשימה מקושרת של כל שורה בקלט,
לכל שורה לוודא שהיא לא ריקה, ושמצאנו כבר את המימד,
לעשות מהשורה נקודה, ולשים את הנקודה ברשימה המקושרת.
ולהעתיק את הרשימה המקושרת לרשימה.

הסיבה לזה שבקלט זורם באופן רציף ואורך מערך צריך להיות קבוע מראש, ולא ציינו שמותר להשתמש בrewind.
"""

def count_dim(line):
    """Counts dimensions based on comma separators."""
    dim = 1
    for char in line:
        if char == '\0' or char == '\n' or char == '\r':
            break
        if char == ',':
            dim += 1
    return dim

def line_to_point(line, dim):
    """Converts a raw string line into a float list point."""
    try:
        tokens = line.strip().split(',')
        if len(tokens) < dim:
            error()
        pt = [float(x) for x in tokens[:dim]]
        return pt
    except ValueError:
        error()

def read_points():
    """Reads points dynamically from standard input."""
    head_list = [] # Replaces the linked list (head/tail/new_node)
    n = 0
    dim = -1
    
    # Simulating standard input streaming line by line
    for line in sys.stdin:
        # לכל שורה, מוודא שהיא לא ריקה ושמצאנו את המימד
        if not line or line[0] in ('\n', '\r', '\0') or line.strip() == "":
            continue
            
        if dim == -1:
            dim = count_dim(line)
            
        # לכל שורה, עושה ממנה נקודה ושם ברשימה המקושרת
        new_point = line_to_point(line, dim)
        head_list.append(new_point)
        n += 1
        
    # מעתיק את הרשימה המקושרת למערך
    pts = []
    for current_pt in head_list:
        pts.append(current_pt)
        
    return pts, n, dim



def parse_positive_int(s, out_container):
    """Parses standard string into integer bounds safety check."""
    if s is None or s == '':
        return False
    try:
        val = int(s)
        if val > 2147483647 or val < -2147483648:
            return False
        out_container[0] = val
        return True
    except ValueError:
        return False

def parse_arguments():
    """Validates parameters given from the sys argv console."""
    iter_val = 400
    k_container = [0]
    iter_container = [400]
    argc = len(sys.argv)
    
    if argc != 2 and argc != 3:
        print("Incorrect number of clusters!")
        sys.exit(1)
        
    if not parse_positive_int(sys.argv[1], k_container) or k_container[0] <= 1:
        print("Incorrect number of clusters!")
        sys.exit(1)
        
    if argc == 3:
        if not parse_positive_int(sys.argv[2], iter_container) or iter_container[0] <= 1 or iter_container[0] >= 800:
            print("Incorrect maximum iteration!")
            sys.exit(1)
        iter_val = iter_container[0]
            
    return k_container[0], iter_val

def print_output(centroids, k, d):
    """Prints coordinates to 4 decimal points separated by commas."""
    for i in range(k):
        for j in range(d):
            print(f"{centroids[i][j]:.4f}", end="")

            if j < d - 1:
                print(",", end="")
        print()

# ==========================================
# 3. Core Logic Algorithm
# ==========================================

def kmeans(points, k, max_itter, d, N, epsilon):
    """Runs the kmeans algorithm exactly imitating your C loop constructs."""
    # N>0 k>0 max_itter>0 d>0
    # runs the kmeans algorithem.
    check_point_lst(points, d, N)
    
    # Memory allocation layer mapping
    centroids = [None] * k
    clusters = [-1] * N
    
    # Step 1: Initialize centroids (Naive approach: pick the first K points)
    # For production, K-Means++ or random selection is preferred.
    for i in range(k):
        centroids[i] = creat_point(points[i], d)
        clusters[i] = i # Initialize cluster assignments
        
    for iteration in range(max_itter):
        changed = False
        
        # Step 2: Assign each point to the nearest centroid
        for i in range(N):
            min_dist = distance(points[i], centroids[0])
            closest_centroid = 0
            
            for j in range(1, k):
                dist = distance(points[i], centroids[j])
                if dist < min_dist:
                    min_dist = dist
                    closest_centroid = j
                    
            # Track if any point changed its cluster assignment
            if clusters[i] != closest_centroid:
                clusters[i] = closest_centroid
                changed = True
                
        # If no assignments changed, the algorithm has converged early
        if not changed:
            break
            
        # Step 3: Recalculate the centroids based on the new cluster assignments
        #initialize sums and counts for each centroid
        point_counts = [0] * k
        sum_x = [[0] * d for i in range(k)]
        new_centroids = []
        centroid_changes = False
        
        for i in range(k):
            new_centroids.append(creat_point(centroids[i], d)) # Initialize new centroids
            
        for i in range(N):
            c = clusters[i]
            for j in range(d):
                sum_x[c][j] += points[i][j]
            point_counts[c] += 1
            
        # Update centroids
        for i in range(k):
            if point_counts[i] > 0:
                for j in range(d):
                    new_centroids[i][j] = sum_x[i][j] / point_counts[i]
                    
            change = distance(centroids[i], new_centroids[i])
            if change > epsilon:
                centroid_changes = True # Mark that a centroid has changed significantly
                
            copy_point(centroids[i], new_centroids[i]) # Update the centroid
            
        # If no centroids changed significantly, the algorithm has converged
        if not centroid_changes:
            break
            
    # call print_output() to print the final centroids
    print_output(centroids, k, d)
    

# ==========================================
# 4. Main Program Execution entry point
# ==========================================

def main():
    epsilon = 0.001 # Define a small epsilon for convergence check
    
    # Because Python passes primitive ints by value, we capture returns directly
    k, iter_val = parse_arguments()
    
    pts, n, dim = read_points()
    
    if k >= n:
        print("Incorrect number of clusters!")
        sys.exit(1)
        
    kmeans(pts, k, iter_val, dim, n, epsilon)
    

if __name__ == "__main__":
    main()