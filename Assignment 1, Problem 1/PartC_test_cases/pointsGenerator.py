import random
import math

# Generate random coordinates within long double range
def random_generate(num_points):
    points = []
    points.append(num_points)  
    for i in range(num_points):
        x = random.uniform(-1e6, 1e6)  # Random x-coordinate
        y = random.uniform(-1e6, 1e6)  # Random y-coordinate
        points.append((x, y))  
    return points

# Generate random coordinates on a circle
def circle_generate(num_points):
    radius = 30
    points = []
    points.append(num_points)  
    for i in range(num_points):
        # Generate a random angle
        angle = random.uniform(0, 2*math.pi)

        # Calculate x and y coordinates on the circle
        x = radius * math.cos(angle)
        y = radius * math.sin(angle)
        points.append((x, y))  
    return points

# Generate random coordinates within the triangle
def triangle_generate(num_points):
    points = []
    points.append(num_points)

    # Vertices of the triangle
    vertices = [(0, 7), (5, 0), (6, 6)]

    # Include vertices within points
    points.extend(vertices)

    # Bounding box of the triangle
    min_x = min(vertex[0] for vertex in vertices)
    max_x = max(vertex[0] for vertex in vertices)
    min_y = min(vertex[1] for vertex in vertices)
    max_y = max(vertex[1] for vertex in vertices)

    # Generate random points within the bounding box
    for i in range(num_points - 3):
        while True:
            # Generate random point within bounding box
            x = random.uniform(min_x, max_x)
            y = random.uniform(min_y, max_y)

            # Check if point is inside the triangle
            if check_inside(x, y, vertices):
                points.append((x, y))
                break

    return points

# Check if a point (x, y) is inside the triangle defined by its vertices
def check_inside(x, y, vertices):
    # Compute vectors from each vertex to the test point
    vectors = [(vertices[i][0] - x, vertices[i][1] - y) for i in range(3)]

    # Compute dot products of adjacent vectors
    dot_products = [vectors[i][0] * vectors[(i + 1) % 3][1] - 
                    vectors[(i + 1) % 3][0] * vectors[i][1] for i in range(3)]
    
    # Check if dot products have the same sign, indicating the point is inside the triangle
    return all(product >= 0 for product in dot_products) or all(product <= 0 for product in dot_products)

# Writes points to file in specific format
def writefile(points, filename):
    with open(filename, 'w') as f:
        for point in points:
            if isinstance(point, int):
                f.write(str(point) + '\n')
            else:
                # Write coordinates with 16 decimal places
                f.write('{:.16f} {:.16f}\n'.format(point[0], point[1]))  
                
def main():
    # Test 1
    points1 = random_generate(10)
    writefile(points1, 'test1_10.txt')
    points2 = random_generate(1000)
    writefile(points2, 'test1_1,000.txt')
    points3 = random_generate(100000)
    writefile(points3, 'test1_100,000.txt')

    # Test 2
    points1 = circle_generate(10)
    writefile(points1, 'test2_10.txt')
    points2 = circle_generate(1000)
    writefile(points2, 'test2_1,000.txt')
    points3 = circle_generate(100000)
    writefile(points3, 'test2_100,000.txt')

    # Test 3
    points1 = triangle_generate(10)
    writefile(points1, 'test3_10.txt')
    points2 = triangle_generate(1000)
    writefile(points2, 'test3_1,000.txt')
    points3 = triangle_generate(100000)
    writefile(points3, 'test3_100,000.txt')
    
# Execute main function
main()
