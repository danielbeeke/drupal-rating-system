#Rating system documentation

##Requirements:
Attach multiple formulas that can have tokens to an entity and calculate the out come.
Formulas should have revisions so you can check out what was the revision outcome.
Calculation should be done within a given timespan.

Formulas should be Drupal entities.
That way we can export them and keep sets of formulas (in a feature) for later reference.

It would be great to display a formula with example data or an example feature.
It would be great to put different formulas in a table to check the difference.

Caching should be possible with different strengths of cache invalidation.
Terms

###Dataset
A dataset contains data within a timespan for entities. It could be pageviews, fields or fivestar ratings etc. Each different dataset has it’s own rating system sub module.

###Formula
A formula is a calculation with tokens. These tokens will be filled with the data selected from the entity that is calculated. Example: [node:view-count] * 3 * [node:view-count-last-month]. 
