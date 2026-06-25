    /*
    הרבה חלקים בשלד הקוד כוללים הערות לבדיקת תקינות הקוד תוך כדי ריצה,
יש לציין שהחלק הזה לא "הכרחי" בריצה תקינה, ונכתב על ידי בינה כדי לתפוס שגיאות תוך כדי טסטים
בכתיבת שלד הקוד היה שימוש בבינה, אך היא לא כתבה את הלוגיקה של 
main, kmeans או point.
האלגוריתם עצמו, רק את השלד והבדיקות.



    */
    
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    void print_output(void);
    void error(void);

    struct point
    {
        
        int dim;
        double *p;
    };

    struct point_node /* Linked List כמו שראינו בשיעור*/
    {
        struct point pt;
        struct point_node *next;
    };

    void error(void)
    {
        printf("An Error Has Occurred\n");
        exit(1);
    }

    struct point creat_point(double lst[], int d)
    {
        struct point pt;
        int i;

        pt.dim = d;

        pt.p = (double *)malloc(d * sizeof(double));

        if (pt.p == NULL) {
            error();
        }

        for (i = 0; i < d; i++) {
            pt.p[i] = lst[i];
        }

        return pt;
    }

    void delete_point(struct point *pt)
    {
        /* frees the space */
        (*pt).dim = 0;
        free((*pt).p);
        (*pt).p = NULL;
    }

    void copy_point(struct point *dest, struct point *src){
    // Copy the contents of src to dest
    // assumes equal dimensions and that dest has allocated memory
    for (int i = 0; i < src->dim; i++) {
        dest->p[i] = src->p[i];
    }
}

    double distance(struct point pt1, struct point pt2)
    {
        int d;
        double *l1;
        double *l2;
        double s;
        double diff;
        double dist;
        int i;

        d = pt1.dim;

        if (pt2.dim != d)
        {
            /* אף פעם לא אמור לקרות*/
            printf("points arent the same dim\n");
            error();
        }

        l1 = pt1.p;
        l2 = pt2.p;
        s = 0.0;

        for (i = 0; i < d; i++)
        {
            diff = l1[i] - l2[i];
            s += diff * diff;
        }

        dist = sqrt(s);
        return dist;
    }
    /* :הרעיון של עיבוד הקלט פשוט יחסית
    להגדיר רשימה מקושרת של כל שורה בקלט,
    לכל שורה לוודא שהיא לא ריקה, ושמצאנו כבר את המימד,
     לעשות מהשורה נקודה, ולשים את הנקודה ברשימה המקושרת.
    ולהעתיק את הרשימה המקושרת לרשימה.
    */
    int count_dim(char line[])
    {
        int i;
        int dim;

        dim = 1;

        for (i = 0; line[i] != '\0' && line[i] != '\n' && line[i] != '\r'; i++) {
            if (line[i] == ',') {
                dim++;
            }
        }

        return dim;
    }

    struct point line_to_point(char line[], int dim)
    {
        struct point pt;
        char *p;
        char *end;
        int i;

        pt.dim = dim;
        pt.p = malloc(dim * sizeof(double));

        if (pt.p == NULL) {
            error();
        }

        p = line;

        for (i = 0; i < dim; i++) {
            pt.p[i] = strtod(p, &end);

            if (end == p) {
                error();
            }

            p = end;

            if (*p == ',') {
                p++;
            }
        }

        return pt;
    }

    struct point *read_points(int *n_out, int *dim_out)
    {
        char line[4096];
        struct node *head;
        struct node *tail;
        struct node *new_node;
        struct node *current;
        struct node *next;
        struct point *pts;
        int n;
        int dim;
        int i;

        /* מגדיר רשימה מקושרת */
        head = NULL;
        tail = NULL;
        n = 0;
        dim = -1;

        while (fgets(line, sizeof(line), stdin) != NULL) {

            /* לכל שורה, מוודא שהיא לא ריקה ושמצאנו את המימד */
            if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
                continue;
            }

            if (dim == -1) {
                dim = count_dim(line);
            }

            /* לכל שורה, עושה ממנה נקודה ושם ברשימה המקושרת */
            new_node = malloc(sizeof(struct node));

            if (new_node == NULL) {
                error();
            }

            new_node->pt = line_to_point(line, dim);
            new_node->next = NULL;

            if (head == NULL) {
                head = new_node;
                tail = new_node;
            }
            else {
                tail->next = new_node;
                tail = new_node;
            }

            n++;
        }

        /* מעתיק את הרשימה המקושרת למערך */
        pts = malloc(n * sizeof(struct point));

        if (pts == NULL) {
            error();
        }

        current = head;
        i = 0;

        while (current != NULL) {
            pts[i] = current->pt;

            next = current->next;
            free(current);
            current = next;

            i++;
        }

        *n_out = n;
        *dim_out = dim;

        return pts;
    }

void kmeans(struct point points[],int k, int max_itter, int d, int N,double epsilon){
    // N>0  k>0  max_itter>0  d>0
    // runs the kmeans algorithem.
    check_point_lst(points,d,N);
    struct point centroids[k];
    int clusters[N];

    for (int i = 0; i < N; i++) {
         clusters[i] = -1; 
    }



    // Step 1: Initialize centroids (Naive approach: pick the first K points)
    // For production, K-Means++ or random selection is preferred.
    for (int i = 0; i < k; i++) {
        struct point p = creat_point(points[i].p, d);
        centroids[i] = p;
        clusters[i] = i;  // Initialize cluster assignments
    }

    for (int iter = 0; iter < max_itter; iter++) {
        int changed = 0;

        // Step 2: Assign each point to the nearest centroid
        for (int i = 0; i < N; i++) {
            double min_dist = distance(points[i], centroids[0]);
            int closest_centroid = 0;

            for (int j = 1; j < k; j++) {
                double dist = distance(points[i], centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = j;
                }
            }

            // Track if any point changed its cluster assignment
            if (clusters[i] != closest_centroid) {
                clusters[i] = closest_centroid;
                changed = 1;
            }
        }

        // If no assignments changed, the algorithm has converged early
        if (!changed) {
            printf("Converged early at iteration %d\n", iter + 1);
            break;
        }

        // Step 3: Recalculate the centroids based on the new cluster assignments        
        int *point_counts = (int *)calloc(k, sizeof(int));
        double (*sum_x)[d] = calloc(k, sizeof(*sum_x)); // Initialize sum arrays for each dimension
        struct point *new_centroids = (struct point *)malloc(k * sizeof(struct point));
        int centroid_changes=0;

        for (int i = 0; i < k; i++) {
            new_centroids[i] = creat_point(centroids[i].p, d); // Initialize new centroids
        }

        for (int i = 0; i < N; i++) {
            int c = clusters[i];
            
            for (int j = 0; j < d; j++) {
                sum_x[c][j] += points[i].p[j];
            }
            point_counts[c]++;
        }

        // Update centroids
        for (int i = 0; i < k; i++) {
            if (point_counts[i] > 0) {
                for (int j = 0; j < d; j++) {
                    new_centroids[i].p[j] = sum_x[i][j] / point_counts[i];
                }
            }
            double change = distance(centroids[i], new_centroids[i]);
            if (change > epsilon) {
                centroid_changes=1; // Mark that a centroid has changed significantly
            }
            copy_point(&centroids[i], &new_centroids[i]); // Update the centroid
            delete_point(&new_centroids[i]); // Free the old centroid memory
        }

        // Free temporary tracking arrays
        free(sum_x);
        free(point_counts);
        free(new_centroids);

        // If no centroids changed significantly, the algorithm has converged
        if (!centroid_changes) {
            printf("Centroids converged at iteration %d\n", iter + 1);
            break;
        }
    }


    //call print_output() to print the final centroids and cluster assignments
    //print_output(centroids, clusters, k, N, d);
    printf("done:\n");


    // Clean up allocated memory for centroids
    for (int i = 0; i < k; i++) {
        delete_point(&centroids[i]);
    }
}


    int main(int argc, char *argv[])
    {
        int k;
        int iter;
        int n;
        int dim;
        struct point *pts;
        double epsilon = 0.001; // Define a small epsilon for convergence check

        parse k and iter from argv;

        read all points from stdin into pts;
        find n and dim;

        validate that 1 < k < n;

        run kmeans(pts, k, iter, dim, n, epsilon);

        print final centroids;

        free memory;

        return 0;
    }