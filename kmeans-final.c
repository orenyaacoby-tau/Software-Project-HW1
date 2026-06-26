    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    void error(void);

    struct point
    {
        int dim;
        double *p;
    };

    void print_output(struct point centroids[], int k, int d);

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

    void copy_point(struct point *dest, struct point *src)
    {
        int i;

        /* Copy the contents of src to dest */
        /* assumes equal dimensions and that dest has allocated memory */
        for (i = 0; i < src->dim; i++) {
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

    void check_point_lst(struct point points[], int d, int N)
    {
        int i;

        if (points == NULL || d <= 0 || N <= 0) {
            error();
        }

        for (i = 0; i < N; i++) {
            if (points[i].dim != d || points[i].p == NULL) {
                error();
            }
        }
    }

    /* :הרעיון של עיבוד הקלט פשוט יחסית
    להגדיר רשימה מקושרת של כל שורה בקלט,
    לכל שורה לוודא שהיא לא ריקה, ושמצאנו כבר את המימד,
     לעשות מהשורה נקודה, ולשים את הנקודה ברשימה המקושרת.
    ולהעתיק את הרשימה המקושרת לרשימה.

    הסיבה לזה שבקלט זורם באופן רציף ואורך מערך צריך להיות קבוע מראש, ולא ציינו שמותר להשתמש בrewind.
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
                delete_point(&pt);
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
        struct point_node *head;
        struct point_node *tail;
        struct point_node *new_node;
        struct point_node *current;
        struct point_node *next;
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
            new_node = malloc(sizeof(struct point_node));

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

    void free_points(struct point *pts, int n)
    {
        int i;

        if (pts == NULL) {
            return;
        }

        for (i = 0; i < n; i++) {
            delete_point(&pts[i]);
        }

        free(pts);
    }

    int parse_positive_int(char *s, int *out)
    {
        char *end;
        long val;

        if (s == NULL || *s == '\0') {
            return 0;
        }

        val = strtol(s, &end, 10);

        if (*end != '\0') {
            return 0;
        }

        if (val > 2147483647L || val < -2147483647L - 1L) {
            return 0;
        }

        *out = (int)val;
        return 1;
    }

    void parse_arguments(int argc, char *argv[], int *k, int *iter)
    {
        *iter = 400;

        if (argc != 2 && argc != 3) {
            printf("Incorrect number of clusters!\n");
            exit(1);
        }

        if (!parse_positive_int(argv[1], k) || *k <= 1) {
            printf("Incorrect number of clusters!\n");
            exit(1);
        }

        if (argc == 3) {
            if (!parse_positive_int(argv[2], iter) || *iter <= 1 || *iter >= 800) {
                printf("Incorrect maximum iteration!\n");
                exit(1);
            }
        }
    }

    void print_output(struct point centroids[], int k, int d)
    {
        int i;
        int j;

        for (i = 0; i < k; i++) {
            for (j = 0; j < d; j++) {
                printf("%.4f", centroids[i].p[j]);

                if (j < d - 1) {
                    printf(",");
                }
            }

            printf("\n");
        }
    }

    void kmeans(struct point points[], int k, int max_itter, int d, int N, double epsilon)
    {
        struct point *centroids;
        int *clusters;
        int *point_counts;
        double *sum_x;
        struct point *new_centroids;
        int i;
        int j;
        int iter;
        int changed;
        int closest_centroid;
        int centroid_changes;
        int c;
        double min_dist;
        double dist;
        double change;

        /* N>0  k>0  max_itter>0  d>0 */
        /* runs the kmeans algorithem. */
        check_point_lst(points, d, N);

        centroids = (struct point *)malloc(k * sizeof(struct point));
        clusters = (int *)malloc(N * sizeof(int));

        if (centroids == NULL || clusters == NULL) {
            free(centroids);
            free(clusters);
            error();
        }

        for (i = 0; i < N; i++) {
             clusters[i] = -1; 
        }

        /* Step 1: Initialize centroids (Naive approach: pick the first K points) */
        /* For production, K-Means++ or random selection is preferred. */
        for (i = 0; i < k; i++) {
            centroids[i] = creat_point(points[i].p, d);
            clusters[i] = i;  /* Initialize cluster assignments */
        }

        for (iter = 0; iter < max_itter; iter++) {
            changed = 0;

            /* Step 2: Assign each point to the nearest centroid */
            for (i = 0; i < N; i++) {
                min_dist = distance(points[i], centroids[0]);
                closest_centroid = 0;

                for (j = 1; j < k; j++) {
                    dist = distance(points[i], centroids[j]);
                    if (dist < min_dist) {
                        min_dist = dist;
                        closest_centroid = j;
                    }
                }

                /* Track if any point changed its cluster assignment */
                if (clusters[i] != closest_centroid) {
                    clusters[i] = closest_centroid;
                    changed = 1;
                }
            }

            /* If no assignments changed, the algorithm has converged early */
            if (!changed) {
                break;
            }

            /* Step 3: Recalculate the centroids based on the new cluster assignments */        
            point_counts = (int *)calloc(k, sizeof(int));
            sum_x = (double )calloc(k * d, sizeof(double)); / Initialize sum arrays for each dimension */
            new_centroids = (struct point *)malloc(k * sizeof(struct point));
            centroid_changes = 0;

            if (point_counts == NULL || sum_x == NULL || new_centroids == NULL) {
                free(point_counts);
                free(sum_x);
                free(new_centroids);
                error();
            }

            for (i = 0; i < k; i++) {
                new_centroids[i] = creat_point(centroids[i].p, d); /* Initialize new centroids */
            }

            for (i = 0; i < N; i++) {
                c = clusters[i];
                
                for (j = 0; j < d; j++) {
                    sum_x[c * d + j] += points[i].p[j];
                }
                point_counts[c]++;
            }

            /* Update centroids */
            for (i = 0; i < k; i++) {
                if (point_counts[i] > 0) {
                    for (j = 0; j < d; j++) {
                        new_centroids[i].p[j] = sum_x[i * d + j] / point_counts[i];
                    }
                }
                change = distance(centroids[i], new_centroids[i]);
                if (change > epsilon) {
                    centroid_changes = 1; /* Mark that a centroid has changed significantly */
                }
                copy_point(&centroids[i], &new_centroids[i]); /* Update the centroid */
                delete_point(&new_centroids[i]); /* Free the old centroid memory */
            }

            /* Free temporary tracking arrays */
            free(sum_x);
            free(point_counts);
            free(new_centroids);

            /* If no centroids changed significantly, the algorithm has converged */
            if (!centroid_changes) {
                break;
            }
        }

        /* call print_output() to print the final centroids */
        print_output(centroids, k, d);

        /* Clean up allocated memory for centroids */
        for (i = 0; i < k; i++) {
            delete_point(&centroids[i]);
        }

        free(centroids);
        free(clusters);
    }

    int main(int argc, char *argv[])
    {
        int k;
        int iter;
        int n;
        int dim;
        struct point *pts;
        double epsilon;

        epsilon = 0.001; /* Define a small epsilon for convergence check */

        parse_arguments(argc, argv, &k, &iter);

        pts = read_points(&n, &dim);

        if (k >= n) {
            free_points(pts, n);
            printf("Incorrect number of clusters!\n");
            return 1;
        }

        kmeans(pts, k, iter, dim, n, epsilon);

        /* print final centroids; */

        free_points(pts, n);

        return 0;
    }